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

#ifndef INCLUDED_EQUALIZERS_MEAS_EVM_CC_IMPL_H
#define INCLUDED_EQUALIZERS_MEAS_EVM_CC_IMPL_H

#include <equalizers/meas_evm_cc.h>

namespace gr {
namespace equalizers {

class meas_evm_cc_impl : public meas_evm_cc {
private:
  gr::digital::constellation_sptr d_cons;
  std::vector<gr_complex> d_cons_points;
  float d_cons_mag;
  float d_cons_mag_sq;
  EvmMeasurementType d_meas_type;
  double mag(gr_complex x);

public:
  meas_evm_cc_impl(gr::digital::constellation_sptr cons,
                   EvmMeasurementType meas_type);
  ~meas_evm_cc_impl();

  // Where all the action really happens
  int work(int noutput_items, gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
};

} // namespace equalizers
} // namespace gr

#endif /* INCLUDED_EQUALIZERS_MEAS_EVM_CC_IMPL_H */
