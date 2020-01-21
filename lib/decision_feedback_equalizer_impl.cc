/* -*- c++ -*- */
/*
 * Copyright 2019, 2020 Perspecta Labs Inc.
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

#include "decision_feedback_equalizer_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

using namespace std;
namespace gr {
namespace equalizers {

decision_feedback_equalizer::sptr
decision_feedback_equalizer::make(int num_taps_forward, int num_taps_feedback,
                                  std::vector<gr_complex> training_sequence,
                                  bool adapt_after_training, int sps,
                                  adaptive_algorithm_sptr alg,
                                  const std::string &training_start_tag) {
  return gnuradio::get_initial_sptr(new decision_feedback_equalizer_impl(
      num_taps_forward, num_taps_feedback, training_sequence,
      adapt_after_training, sps, alg, training_start_tag));
}

/*
 * The private constructor
 */
decision_feedback_equalizer_impl::decision_feedback_equalizer_impl(
    int num_taps_forward, int num_taps_feedback,
    std::vector<gr_complex> training_sequence, bool adapt_after_training,
    int sps, adaptive_algorithm_sptr alg, const std::string &training_start_tag)
    : gr::sync_decimator(
          "decision_feedback_equalizer",
          io_signature::make(1, 1, sizeof(gr_complex)),
          io_signature::makev(
              1, 3,
              std::vector<int>{sizeof(gr_complex),
                               (int)((num_taps_forward + num_taps_feedback) *
                                     sizeof(gr_complex)),
                               sizeof(unsigned short)}),
          sps),
      filter::kernel::fir_filter_ccc(
          sps, vector<gr_complex>(num_taps_forward + num_taps_feedback,
                                  gr_complex(0, 0))),
      d_sps(sps), d_alg(alg), d_num_taps_fwd(num_taps_forward),
      d_num_taps_rev(num_taps_feedback),
      d_num_taps(num_taps_forward + num_taps_feedback),
      d_adapt_after_training(adapt_after_training),
      d_training_sequence(training_sequence),
      d_training_start_tag(training_start_tag),
      // d_taps(num_taps_forward + num_taps_feedback),
      d_decision_history(num_taps_feedback),
      d_new_taps(num_taps_forward + num_taps_feedback) {


  // not reversed at this point
  d_new_taps[0] =
      gr_complex(1.0, 0.0); // help things along with initialized taps

  set_history(d_num_taps_fwd);

  const size_t alignment = volk_get_alignment();
  d_filt_buf =
      (gr_complex *)volk_malloc(sizeof(gr_complex) * d_num_taps, alignment);

  // alg->initialize_taps(d_taps);
  //  set_taps reverses the taps in the filter object
  //  we need to maintain reversed taps in our local copy
  filter::kernel::fir_filter_ccc::set_taps(d_new_taps);

  // initialize the decision history
  // d_decision_history[d_decision_history.size()-1] = gr_complex(1.0,0.0);
}

/*
 * Our virtual destructor.
 */
decision_feedback_equalizer_impl::~decision_feedback_equalizer_impl() {
  volk_free(d_filt_buf);
}

void decision_feedback_equalizer_impl::update_decision_history(
    gr_complex decision) {

  for (size_t d = 0; d < d_decision_history.size() - 1; d++) {
    d_decision_history[d] = d_decision_history[d + 1];
  }

  d_decision_history[d_decision_history.size() - 1] = decision;
}

int decision_feedback_equalizer_impl::work(
    int noutput_items, gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items) {
  const gr_complex *in = (const gr_complex *)input_items[0];
  gr_complex *out = (gr_complex *)output_items[0];

  int outlen = output_items.size();

  unsigned short *state = NULL;
  gr_complex *taps = NULL;

  if (outlen > 1)
    taps = (gr_complex *)output_items[1];
  if (outlen > 3)
    state = (unsigned short *)output_items[2];

  unsigned long int nread = nitems_read(0);
  vector<tag_t> unfilt_tags, tags;
  // get_tags_in_window(tags, 0, 0, noutput_items*decimation(),
  //                    pmt::intern(d_training_start_tag));
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

int decision_feedback_equalizer_impl::process(
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
    vector<gr_complex> in_prepended_history(nin + d_num_taps_fwd - 1);
    std::copy(_in, _in + nin,
              in_prepended_history.begin() + d_num_taps_fwd - 1);
    samples = &in_prepended_history[0];
  }

  unsigned int tag_index = 0;

  int j = 0;

  for (int i = 0; i < nout; i++) {
    memcpy(d_filt_buf + d_num_taps_rev, &samples[j + d_sps - 1],
           sizeof(gr_complex) * d_num_taps_fwd);
    memcpy(d_filt_buf, &d_decision_history[0],
           sizeof(gr_complex) * d_num_taps_rev);

    out[i] = filter(&d_filt_buf[0]);

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
      } else
        d_training_state = EqualizerState::IDLE;

      d_training_sample = -1;
    }

    gr_complex decision = gr_complex(0.0, 0.0);
    // Adjust taps
    if (d_training_state == EqualizerState::TRAINING) {
      decision = d_training_sequence[d_training_sample++];
      d_error = d_alg->error_tr(out[i], decision);
    } else if (d_training_state == EqualizerState::DD) {
      d_error = d_alg->error_dd(out[i], decision); // returns the decision
    }

    // update the decision history
    update_decision_history(decision);

    switch (d_training_state) {
    case EqualizerState::IDLE:
      d_error = gr_complex(0.0, 0.0);
      break;
    case EqualizerState::TRAINING:
    case EqualizerState::DD:
      d_alg->update_taps(&d_taps[0], &d_filt_buf[0], d_error, decision,
                         d_taps.size());
      for (size_t k = 0; k < d_taps.size(); k++) {
        // Update aligned taps in filter object.
        update_tap(d_taps[k], k);
      }

      break;
    }

    j += decimation();
  }

  return nout;
}

} /* namespace equalizers */
} // namespace gr
