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

#ifndef INCLUDED_EQUALIZERS_MEAS_EVM_CC_H
#define INCLUDED_EQUALIZERS_MEAS_EVM_CC_H

#include <equalizers/api.h>
#include <gnuradio/sync_block.h>

#include <gnuradio/digital/constellation.h>

namespace gr {
namespace equalizers {

/*!
 * \brief Measures EVM of incoming symbols relative to a constellation object
 * \ingroup equalizers
 *
 */
enum EvmMeasurementType { EVM_PERCENT = 0, EVM_DB = 1 };

class EQUALIZERS_API meas_evm_cc : virtual public gr::sync_block {
public:
  typedef boost::shared_ptr<meas_evm_cc> sptr;

  /*!
   * \brief Return a shared_ptr to a new instance of equalizers::meas_evm_cc.
   *
   * To avoid accidental use of raw pointers, equalizers::meas_evm_cc's
   * constructor is in a private implementation
   * class. equalizers::meas_evm_cc::make is the public interface for
   * creating new instances.
   */
  static sptr make(gr::digital::constellation_sptr cons,
                   EvmMeasurementType meas_type);
};

} // namespace equalizers
} // namespace gr

#endif /* INCLUDED_EQUALIZERS_MEAS_EVM_CC_H */
