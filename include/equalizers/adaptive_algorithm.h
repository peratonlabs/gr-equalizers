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

#ifndef INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_H
#define INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_H

#include <algorithm>
#include <boost/enable_shared_from_this.hpp>
#include <equalizers/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/math.h>
#include <math.h>
#include <vector>

namespace gr {
namespace equalizers {
enum class AlgorithmType { LMS, NLMS, RLS, CMA, ZF };

class adaptive_algorithm;
typedef boost::shared_ptr<adaptive_algorithm> adaptive_algorithm_sptr;

class EQUALIZERS_API adaptive_algorithm
    : public boost::enable_shared_from_this<adaptive_algorithm> {
protected:
  AlgorithmType algorithm_type;
  gr::digital::constellation_sptr constellation;

public:
  virtual ~adaptive_algorithm() {}

  adaptive_algorithm(AlgorithmType type, gr::digital::constellation_sptr cons) {
    this->algorithm_type = type;
    this->constellation = cons;
  }
  adaptive_algorithm_sptr base() { return shared_from_this(); }

  virtual void initialize_taps(std::vector<gr_complex> &taps) {
    std::fill(taps.begin(), taps.end(), gr_complex(0.0, 0.0));
    taps[0] = gr_complex(1.0, 0.0);
  }

  virtual gr_complex error_dd(gr_complex &wu, gr_complex &decision) {
    gr_complex err;
    // The `map_to_points` function will treat `decision` as an array pointer.
    // This call is "safe" because `map_to_points` is limited by the
    // dimensionality of the constellation. This class calls the
    // `constellation` class default constructor, which initializes the
    // dimensionality value to `1`. Thus, Only the single `gr_complex` value
    // will be dereferenced.
    constellation->map_to_points(constellation->decision_maker(&wu), &decision);
    err = decision - wu;

    return err;
  }

  virtual gr_complex error_tr(gr_complex &wu, gr_complex &d_n) {
    gr_complex err;
    err = d_n - wu;

    return err;
  }

  virtual void update_tap(gr_complex &tap, const gr_complex &in,
                          const gr_complex error,
                          const gr_complex decision) = 0;

  virtual void update_taps(gr_complex *taps, const gr_complex *in,
                           const gr_complex error, const gr_complex decision,
                           unsigned int num_taps) {
    // default tap update until update_taps is implemented for all the alg types
    for (unsigned i = 0; i < num_taps; i++) {
      this->update_tap(taps[i], in[i], error, decision);
    }
  }
};

} // namespace equalizers
} // namespace gr
#endif