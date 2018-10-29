/* -*- c++ -*- */
/* 
 * Copyright 2018 RuofanLiang@SJTU.
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

#ifndef INCLUDED_ZIGBEE_OQPSK_ACCESS_CODE_PREFIXER_IMPL_H
#define INCLUDED_ZIGBEE_OQPSK_ACCESS_CODE_PREFIXER_IMPL_H

#include <zigbee_OQPSK/access_code_prefixer.h>

namespace gr {
  namespace zigbee_OQPSK {

    class access_code_prefixer_impl : public access_code_prefixer
    {
     private:
      // Nothing to declare in this block.

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      access_code_prefixer_impl(pad,preamble);
      ~access_code_prefixer_impl();

      // Where all the action really happens
      int work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace zigbee_OQPSK
} // namespace gr

#endif /* INCLUDED_ZIGBEE_OQPSK_ACCESS_CODE_PREFIXER_IMPL_H */

