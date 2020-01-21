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

#ifndef INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_ZF_H
#define INCLUDED_EQUALIZERS_ADAPTIVE_ALGORITHM_ZF_H

#include <equalizers/adaptive_algorithm.h>

namespace gr {
namespace equalizers {

class EQUALIZERS_API adaptive_algorithm_zf : public adaptive_algorithm {
public:
  typedef boost::shared_ptr<adaptive_algorithm_zf> sptr;

protected:
  adaptive_algorithm_zf(gr::digital::constellation_sptr cons)
      : adaptive_algorithm(AlgorithmType::ZF, cons) {}

public:
  static sptr make(gr::digital::constellation_sptr cons) {
    return adaptive_algorithm_zf::sptr(new adaptive_algorithm_zf(cons));
  }

  void update_tap(gr_complex &tap, const gr_complex &I_n, const gr_complex err,
                  const gr_complex decision) {

    tap = conj(conj(tap) + conj(decision) * err);
  }

  ~adaptive_algorithm_zf() {}
  AlgorithmType algorithm_type;

};

} // namespace equalizers
} // namespace gr

#endif
