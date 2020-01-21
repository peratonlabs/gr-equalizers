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

#ifndef INCLUDED_EQUALIZERS_DECISION_FEEDBACK_EQUALIZER_H
#define INCLUDED_EQUALIZERS_DECISION_FEEDBACK_EQUALIZER_H

#include <equalizers/api.h>
#include <gnuradio/sync_decimator.h>

#include <equalizers/adaptive_algorithm.h>

namespace gr {
  namespace equalizers {

    /*!
     * \brief <+description of block+>
     * \ingroup equalizers
     *
     */
    class EQUALIZERS_API decision_feedback_equalizer : virtual public gr::sync_decimator
    {
     public:
      typedef boost::shared_ptr<decision_feedback_equalizer> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of equalizers::decision_feedback_equalizer.
       *
       * To avoid accidental use of raw pointers, equalizers::decision_feedback_equalizer's
       * constructor is in a private implementation
       * class. equalizers::decision_feedback_equalizer::make is the public interface for
       * creating new instances.
       */
      static sptr make(int num_taps_forward, int num_taps_feedback, 
                   std::vector<gr_complex> training_sequence,
                   bool adapt_after_training,
                   int sps,
                   adaptive_algorithm_sptr alg,
                   const std::string& training_start_tag="" );
    };

  } // namespace equalizers
} // namespace gr

#endif /* INCLUDED_EQUALIZERS_DECISION_FEEDBACK_EQUALIZER_H */

