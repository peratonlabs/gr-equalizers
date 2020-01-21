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

#ifndef INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_CMA_H
#define INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_CMA_H

#include <equalizers/adaptive_algorithm.h>

namespace gr {
namespace equalizers {
class EQUALIZERS_API adaptive_algorithm_cma : public adaptive_algorithm {
public:
  typedef boost::shared_ptr<adaptive_algorithm_cma> sptr;

  float step_size;
  float modulus;

protected:
  adaptive_algorithm_cma(gr::digital::constellation_sptr cons, float step_size,
                         float modulus)
      : adaptive_algorithm(AlgorithmType::CMA, cons) {
    this->step_size = step_size;
    this->modulus = modulus;
  }

public:
  static sptr make(gr::digital::constellation_sptr cons, float step_size,
                   float modulus) {
    return adaptive_algorithm_cma::sptr(
        new adaptive_algorithm_cma(cons, step_size, modulus));
  }
  gr_complex error(const gr_complex &out) {
    gr_complex error = out * (norm(out) - modulus);
    float re = gr::clip(error.real(), 1.0);
    float im = gr::clip(error.imag(), 1.0);
    return gr_complex(re, im);
  }

  gr_complex error_dd(gr_complex &u_n, gr_complex &decision) { return error(u_n); }

  gr_complex error_tr(gr_complex &u_n, gr_complex &d_n) { return error(u_n); }

  void update_taps(gr_complex *taps, const gr_complex *in,
                   const gr_complex error, const gr_complex decision,
                   unsigned int num_taps) {

    gr_complex *prod_vector;
    gr_complex *conj_vector;

    const size_t alignment = volk_get_alignment();
    prod_vector =
        (gr_complex *)volk_malloc(sizeof(gr_complex) * num_taps, alignment);
    conj_vector =
        (gr_complex *)volk_malloc(sizeof(gr_complex) * num_taps, alignment);

    gr_complex err_x_mu = -step_size * error;

    volk_32fc_conjugate_32fc(conj_vector, in, num_taps);
    volk_32fc_s32fc_multiply_32fc(prod_vector, conj_vector, err_x_mu, num_taps);
    volk_32fc_x2_add_32fc(taps, taps, prod_vector, num_taps);

    volk_free(prod_vector);
    volk_free(conj_vector);
  }

  void update_tap(gr_complex &tap, const gr_complex &u_n,
                  const gr_complex err, const gr_complex decision) {

    tap = conj(conj(tap) - step_size * u_n * conj(err));
  }

  ~adaptive_algorithm_cma() {}
  AlgorithmType algorithm_type;
};

} // namespace equalizers
} // namespace gr

#endif
