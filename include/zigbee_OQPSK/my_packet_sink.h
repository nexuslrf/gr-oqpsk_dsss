/* -*- c++ -*- */
/* 
 * Copyright 2018 packet_sink.
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


#ifndef INCLUDED_ZIGBEE_OQPSK_MY_PACKET_SINK_H
#define INCLUDED_ZIGBEE_OQPSK_MY_PACKET_SINK_H

#include <zigbee_OQPSK/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace zigbee_OQPSK {

    /*!
     * \brief <+description of block+>
     * \ingroup zigbee_OQPSK
     *
     */
    class ZIGBEE_OQPSK_API my_packet_sink : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<my_packet_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of zigbee_OQPSK::my_packet_sink.
       *
       * To avoid accidental use of raw pointers, zigbee_OQPSK::my_packet_sink's
       * constructor is in a private implementation
       * class. zigbee_OQPSK::my_packet_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(unsigned int threshold = 10,unsigned int dsss_mode=32,
          int verbose=0,int verbose2=0);
    };

  } // namespace zigbee_OQPSK
} // namespace gr

#endif /* INCLUDED_ZIGBEE_OQPSK_MY_PACKET_SINK_H */

