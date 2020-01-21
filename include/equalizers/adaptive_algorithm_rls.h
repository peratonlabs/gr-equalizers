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

#ifndef INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_RLS_H
#define INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_RLS_H

#include <equalizers/adaptive_algorithm.h>
#include <equalizers/arma_types.h>

namespace gr {
namespace equalizers {
class EQUALIZERS_API adaptive_algorithm_rls : public adaptive_algorithm {
public:
  typedef boost::shared_ptr<adaptive_algorithm_rls> sptr;

  float forgetting_factor; // typically between 0.98 and 1.00
  float delta;             // for initializing P

  cf_mat P;

protected:
  adaptive_algorithm_rls(gr::digital::constellation_sptr cons,
                         float forgetting_factor, float delta)
      : adaptive_algorithm(AlgorithmType::RLS, cons) {
    this->forgetting_factor = forgetting_factor;
    this->delta = delta;
  }

public:
  static sptr make(gr::digital::constellation_sptr cons, float forgetting_factor, float delta) {
    return adaptive_algorithm_rls::sptr(
        new adaptive_algorithm_rls(cons, forgetting_factor, delta));
  }
  void update_tap(gr_complex &tap, const gr_complex &in,
                          const gr_complex error, const gr_complex decision)
                          {
                              // don't use this one
                          }

  void update_taps(gr_complex *taps, const gr_complex *u_n,
                  const gr_complex err, const gr_complex decision, unsigned int num_taps) {

        float l = 1.0/forgetting_factor;
        
        cf_vec uvec(num_taps);
        memcpy(uvec.memptr(), u_n, sizeof(gr_complex)*num_taps);
        
        cf_vec a = P*uvec;
        gr_complex b = cdot(uvec,a);
        // cf_vec k = l*P*uvec / (1.0 + l*uvec.t()*P*uvec)[0];
        cf_vec k = l*a / ((float)1.0 + l*b);
        
        for (int i=0; i<num_taps; i++)
        {
            taps[i] = conj( conj(taps[i]) + k[i]*conj(err)  );
        }

        P = l*P - l*k*uvec.t()*P;
  }

  ~adaptive_algorithm_rls() {}
  AlgorithmType algorithm_type;

  void initialize_taps(std::vector<gr_complex> &taps) {
    std::fill(taps.begin(), taps.end(), gr_complex(0.0, 0.0));

    P = cf_mat(taps.size(), taps.size(), fill::eye)*(1.0/delta);
  }
};
} // namespace equalizers
} // namespace gr

#endif
