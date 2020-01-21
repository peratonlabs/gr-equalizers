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

#ifndef INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_NLMS_H
#define INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_NLMS_H

#include <equalizers/adaptive_algorithm.h>
#include <volk/volk.h>

namespace gr {
namespace equalizers {
  
class EQUALIZERS_API adaptive_algorithm_nlms : public adaptive_algorithm {
public:
  typedef boost::shared_ptr<adaptive_algorithm_nlms> sptr;

  float modulus;
  float step_size;

protected:
  adaptive_algorithm_nlms(gr::digital::constellation_sptr cons, float step_size)
      : adaptive_algorithm(AlgorithmType::NLMS, cons) {
    this->step_size = step_size;
  }

public:
  static sptr make(gr::digital::constellation_sptr cons, float step_size) {
    return adaptive_algorithm_nlms::sptr(
        new adaptive_algorithm_nlms(cons, step_size));
  }
void update_tap(gr_complex &tap, const gr_complex &in,
                          const gr_complex error,
                          const gr_complex decision)
                          {
                            throw "this should not get called";
                          }

  void update_taps(gr_complex *taps, const gr_complex *in,
                           const gr_complex error, const gr_complex decision,
                           unsigned int num_taps) {
                          
    gr_complex dp;
    volk_32fc_x2_conjugate_dot_prod_32fc(&dp, in, in, num_taps);
    float magsq = abs(dp);
    float norm_step_size = step_size / magsq;

    for (unsigned i = 0; i < num_taps; i++) {
      taps[i] = conj(conj(taps[i]) + norm_step_size * in[i] * conj(error));
    }
  }

  ~adaptive_algorithm_nlms() {}
  AlgorithmType algorithm_type;

  void initialize_taps(std::vector<gr_complex> &taps) {
    std::fill(taps.begin(), taps.end(), gr_complex(0.0, 0.0));
  }
};

} // namespace equalizers
} // namespace gr

#endif