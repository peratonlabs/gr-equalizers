/* -*- c++ -*- */
/*
 * Copyright 2019, 2020 Perspecta Labs Inc..
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "linear_equalizer_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/misc.h>
#include <vector>
#include <volk/volk.h>

#include <equalizers/adaptive_algorithm_cma.h>
#include <equalizers/adaptive_algorithm_lms.h>
#include <equalizers/adaptive_algorithm_rls.h>

using namespace pmt;
using namespace std;

namespace gr {
namespace equalizers {

linear_equalizer::sptr
linear_equalizer::make(int num_taps, vector<gr_complex> training_sequence,
                       bool adapt_after_training, int sps,
                       adaptive_algorithm_sptr alg,
                       const std::string &training_start_tag) {
  return gnuradio::get_initial_sptr(new linear_equalizer_impl(
      num_taps, training_sequence, adapt_after_training, sps, alg,
      training_start_tag));
}

/*
 * The private constructor
 */
linear_equalizer_impl::linear_equalizer_impl(
    int num_taps, vector<gr_complex> training_sequence,
    bool adapt_after_training, int sps, adaptive_algorithm_sptr alg,
    const std::string &training_start_tag)
    : gr::sync_decimator("linear_equalizer",
                         io_signature::make(1, 1, sizeof(gr_complex)),
                         io_signature::make3(1, 3, sizeof(gr_complex),
                                             num_taps * sizeof(gr_complex),
                                             sizeof(unsigned short)),
                         sps),
      filter::kernel::fir_filter_ccc(
          sps, vector<gr_complex>(num_taps, gr_complex(0, 0))),
      d_alg(alg), d_updated(false) {

  d_training_sequence = training_sequence;
  // d_initial_weights = initial_weights;
  d_filter = pmt::intern(training_start_tag);
  d_training_start_tag = training_start_tag;
  d_adapt_after_training = adapt_after_training;
  d_sps = sps;
  d_num_taps = num_taps;
  d_new_taps.resize(num_taps);

  if (training_start_tag == "" || training_sequence.size() == 0) {
    d_training_state = EqualizerState::DD;
  } else {
    d_training_state = EqualizerState::IDLE;
  }

  d_training_sample = 0;

  alg->initialize_taps(d_new_taps);
  // NOTE: the filter kernel reverses the taps internally
  filter::kernel::fir_filter_ccc::set_taps(d_new_taps);

  const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
  set_alignment(max(1, alignment_multiple));

  set_history(num_taps);
}

/*
 * Our virtual destructor.
 */
linear_equalizer_impl::~linear_equalizer_impl() {}

void linear_equalizer_impl::set_taps(const vector<gr_complex> &taps) {
  d_new_taps = taps;
  d_updated = true;
}

vector<gr_complex> linear_equalizer_impl::taps() const { return d_taps; }

int linear_equalizer_impl::process(
    gr_complex *out, const gr_complex *_in, unsigned int nin,
    unsigned int max_nout,
    /* optional outputs */ gr_complex *taps, unsigned short *state,
    /* optional parameters */ bool history_included,
    vector<unsigned int> training_start_samples) {

  int nout = nin / d_sps;
  if (nout > max_nout) {
    nout = max_nout;
  }

  const gr_complex *samples;
  if (history_included) {
    samples = _in;
  } else {
    // When process() is called outside the scheduler, allow zeros to be added
    // as history
    vector<gr_complex> in_prepended_history(nin + d_num_taps - 1);
    std::copy(_in, _in + nin, in_prepended_history.begin() + d_num_taps - 1);
    samples = &in_prepended_history[0];
  }

  unsigned int tag_index = 0;

  int j = 0;
  size_t k, l = d_taps.size();
  for (int i = 0; i < nout; i++) {

    out[i] = filter(&samples[j]);

    if (taps)
      memcpy(&taps[d_num_taps * i], &d_taps[0],
             d_num_taps * sizeof(gr_complex));

    if (state)
      state[i] = (unsigned short)d_training_state;

    if (training_start_samples.size() > 0 &&
        tag_index < training_start_samples.size()) {
      int tag_sample = training_start_samples[tag_index];
      if (tag_sample >= j && tag_sample < (j + decimation())) {
        d_training_state = EqualizerState::TRAINING;
        d_training_sample = 0;
        tag_index++;
      }
    }

    // Are we done with the training sequence
    if (d_training_sample >= d_training_sequence.size()) {
      if (d_adapt_after_training) {
        d_training_state = EqualizerState::DD;
      } else {
        d_training_state = EqualizerState::IDLE;
      }
      d_training_sample = -1;
    }

    // Adjust taps
    if (d_training_state == EqualizerState::TRAINING) {
      d_decision = d_training_sequence[d_training_sample];
      d_error =
          d_alg->error_tr(out[i], d_training_sequence[d_training_sample++]);
    } else if (d_training_state == EqualizerState::DD) {
      d_error = d_alg->error_dd(out[i], d_decision);
    }
    switch (d_training_state) {
    case EqualizerState::IDLE:
      d_error = gr_complex(0.0, 0.0);
      break;
    case EqualizerState::TRAINING:
    case EqualizerState::DD:
      d_alg->update_taps(&d_taps[0], &samples[j], d_error, d_decision, l);
      for (k = 0; k < l; k++) {
        // Update aligned taps in filter object.
        filter::kernel::fir_filter_ccc::update_tap(d_taps[k], k);
      }
      break;
    }

    j += decimation();
  }

  return nout;
}

int linear_equalizer_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items) {
  const gr_complex *in = (const gr_complex *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  int outlen = output_items.size();

  unsigned short *state = NULL;
  gr_complex *taps = NULL;

  if (outlen > 1)
    taps = (gr_complex *)output_items[1];
  if (outlen > 2)
    state = (unsigned short *)output_items[2];

  if (d_updated) {
    d_taps = d_new_taps;
    set_history(d_taps.size());
    d_updated = false;
    return 0; // history requirements may have changed.
  }

  unsigned long int nread = nitems_read(0);
  vector<tag_t> unfilt_tags, tags;

  // get_tags_in_window(tags, 0, 0, noutput_items*decimation(),
  //                    pmt::intern(d_training_start_tag));
  // TODO: figure out why filtering on specific tags was not working
  uint64_t start_range = nread;
  uint64_t end_range = nread + (uint64_t)(noutput_items * decimation());
  vector<unsigned int> training_start_samples;
  get_tags_in_range(unfilt_tags, 0, start_range, end_range);
  for (unsigned i = 0; i < unfilt_tags.size(); i++) {
    if (symbol_to_string(unfilt_tags[i].key) == d_training_start_tag) {
      tags.push_back(unfilt_tags[i]);
      training_start_samples.push_back(unfilt_tags[i].offset - nread);
    }
  }

  return process(out, in, noutput_items * decimation(), noutput_items, taps,
                 state, true, training_start_samples);
}

} /* namespace equalizers */
} /* namespace gr */
