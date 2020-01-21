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

#include "meas_evm_cc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace equalizers {

meas_evm_cc::sptr meas_evm_cc::make(gr::digital::constellation_sptr cons,
                                    EvmMeasurementType meas_type) {
  return gnuradio::get_initial_sptr(new meas_evm_cc_impl(cons, meas_type));
}

/*
 * The private constructor
 */
meas_evm_cc_impl::meas_evm_cc_impl(gr::digital::constellation_sptr cons,
                                   EvmMeasurementType meas_type)
    : gr::sync_block("meas_evm_cc",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(float))),
      d_cons(cons), d_meas_type(meas_type) {

  d_cons_points = d_cons->points();

  double sum = 0.0;
  int N = d_cons_points.size();
  for (int i = 0; i < N; i++) {
    sum += pow(real(d_cons_points[i]), 2.0) + pow(imag(d_cons_points[i]), 2.0);
  }
  sum /= (double)N;
  d_cons_mag_sq = sum;
  d_cons_mag = sqrt(sum);
}

double meas_evm_cc_impl::mag(gr_complex x) {
  return sqrt(pow(real(x), 2.0) + pow(imag(x), 2.0));
}

/*
 * Our virtual destructor.
 */
meas_evm_cc_impl::~meas_evm_cc_impl() {}

int meas_evm_cc_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items) {
  const gr_complex *in = (const gr_complex *)input_items[0];
  float *out = (float *)output_items[0];

  // Compare incoming symbols to the constellation decision points
  for (int s = 0; s < noutput_items; s++) {
    gr_complex u_n = in[s];
    gr_complex decision;
    d_cons->map_to_points(d_cons->decision_maker(&u_n), &decision);
    double err_mag = std::abs(decision - u_n);

    if (d_meas_type == EVM_PERCENT) {
      out[s] = (err_mag / d_cons_mag) * 100.0;
    } else {
      out[s] = 20 * log10(err_mag / d_cons_mag);
    }
  }

  // Tell runtime system how many output items we produced.
  return noutput_items;
}

} /* namespace equalizers */
} /* namespace gr */
