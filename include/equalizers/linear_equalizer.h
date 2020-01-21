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

#ifndef INCLUDED_EQUALIZERS_LINEAR_EQUALIZER_H
#define INCLUDED_EQUALIZERS_LINEAR_EQUALIZER_H

#include <equalizers/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/sync_decimator.h>
#include <equalizers/adaptive_algorithm.h>

#include <vector>
#include <string>

namespace gr {
namespace equalizers {

/*!
 * \brief Linear Equalizer block provides linear equalization using a specified adaptive algorithm
 * \ingroup equalizers
 *
 */
class EQUALIZERS_API linear_equalizer : virtual public gr::sync_decimator {

public:
  typedef boost::shared_ptr<linear_equalizer> sptr;

  /*!
   * \brief Return a shared_ptr to a new instance of
   * equalizers::linear_equalizer.
   *
   * The Linear Equalizer block equalizes the incoming signal using an FIR filter.
   * This block is heavily based on the GNU Radio LMS DD block
   * If provided with a training sequence and a training start tag, data aided equalization 
   * will be performed starting with the tagged sample.
   * If training-based equalization is active and the training sequence ends, then optionally
   * decision directed equalization will be performed given the adapt_after_training
   * If no training sequence or no tag is provided, decision directed equalization will be
   * performed
   * This equalizer decimates to the symbol rate according to the samples per symbol param
   * 
   * \param num_taps  Number of taps for the FIR filter
   * \param training_sequence  Sequence of samples that will be used to train the equalizer.  Provide empty vector to default to DD equalizer
   * \param adapt_after_training  bool - set true to continue DD training after training sequence has been used up
   * \param sps  int - Samples per Symbol 
   * \param alg  Adaptive algorithm object
   * \param training_start_tag  string - tag that indicates the start of the training sequence in the incoming data
   */
  static sptr make(int num_taps, 
                   std::vector<gr_complex> training_sequence,
                   bool adapt_after_training,
                   int sps,
                   adaptive_algorithm_sptr alg,
                   const std::string& training_start_tag="");
  virtual void set_taps(const std::vector<gr_complex> &taps) = 0;
  virtual std::vector<gr_complex> taps() const = 0;

  virtual int process(gr_complex *out, const gr_complex *_in, unsigned int nin, unsigned int max_nout,
            /* optional outputs */ gr_complex *taps = NULL, unsigned short *state = NULL, 
            /* optional parameters */ bool history_included = false, std::vector<unsigned int> training_start_samples = std::vector<unsigned int>(0) ) = 0;
};

} // namespace equalizers
} // namespace gr

#endif /* INCLUDED_EQUALIZERS_LINEAR_EQUALIZER_H */
