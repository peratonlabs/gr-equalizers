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

#ifndef INCLUDED_EQUALIZERS_LINEAR_EQUALIZER_IMPL_H
#define INCLUDED_EQUALIZERS_LINEAR_EQUALIZER_IMPL_H

#include <equalizers/linear_equalizer.h>
#include <gnuradio/filter/fir_filter.h>
#include <stdexcept>
#include "equalizer.h"

namespace gr {
namespace equalizers {

// Equalizer States: 
//  IDLE -- just FIR filtering using the current taps
//  TRAINING -- calculating adaptive taps based on training sequence
//  DD -- calculating adaptive taps based on expected constellation points (in algorithm object)



class linear_equalizer_impl : public linear_equalizer,
                              filter::kernel::fir_filter_ccc {
private:
  std::vector<gr_complex> d_new_taps;
  bool d_updated;
  gr_complex d_error;
  gr_complex d_decision;
  std::vector<gr_complex> d_training_sequence;
  std::vector<gr_complex> d_initial_weights;  // todo - make this specifiable - currently not used
  pmt::pmt_t d_filter;
  std::string d_training_start_tag;
  bool d_adapt_after_training;
  int d_sps;
  int d_num_taps;

  EqualizerState d_training_state;
  int d_training_sample;

  adaptive_algorithm_sptr d_alg;

public:
  linear_equalizer_impl(int num_taps,
                        std::vector<gr_complex> training_sequence,
                        bool adapt_after_training, int sps,
                        adaptive_algorithm_sptr alg,
                        const std::string &training_start_tag = "");
  ~linear_equalizer_impl();

  void set_taps(const std::vector<gr_complex> &taps);
  std::vector<gr_complex> taps() const;

  int work(int noutput_items, gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

  // Same as the work function, but more specific and decoupled from the
  // scheduler-related actions
  int process(gr_complex *out, const gr_complex *_in, unsigned int nin, unsigned int max_nout,
            /* optional outputs */ gr_complex *taps = NULL, unsigned short *state = NULL, 
            /* optional parameters */ bool history_included = false, std::vector<unsigned int> training_start_samples = std::vector<unsigned int>(0) );

};

} // namespace equalizers
} // namespace gr

#endif /* INCLUDED_EQUALIZERS_LINEAR_EQUALIZER_IMPL_H */
