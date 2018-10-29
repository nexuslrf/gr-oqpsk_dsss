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

#include <zigbee_OQPSK/packet_sink.h>
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <gnuradio/blocks/count_bits.h>
#include <iostream>

using namespace gr::zigbee_OQPSK;

// this is the mapping between chips and symbols if we do
// a fm demodulation of the O-QPSK signal. Note that this
// is different than the O-QPSK chip sequence from the
// 802.15.4 standard since there there is a translation
// happening.
// See "CMOS RFIC Architectures for IEEE 802.15.4 Networks",
// John Notor, Anthony Caviglia, Gary Levy, for more details.
static const unsigned long CHIP_MAPPING[4][16] = {
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {
					1618456172,
					1309113062,
					1826650030,
					1724778362,
					778887287,
					2061946375,
					2007919840,
					125494990,
					529027475,
					838370585,
					320833617,
					422705285,
					1368596360,
					85537272,
					139563807,
					2021988657},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    };

static const int MAX_PKT_LEN    = 128 -  1; // remove header and CRC
static const int MAX_LQI_SAMPLES = 8; // Number of chip correlation samples to take

static const unsigned long MASKTABLE1[] = {
    0x7E,
    0x7FFE,
    0x7FFFFFFE,
    0x7FFFFFFFFFFFFFFE
};

static const unsigned long MASKTABLE2[] = {
    0xFE,
    0xFFFE,
    0xFFFFFFFE,
    0xFFFFFFFFFFFFFFFE
};

class packet_sink_impl : public packet_sink {
public:

void enter_search()
{
	if (d_verbose)
		fprintf(stderr, "@ enter_search\n");

	d_state = STATE_SYNC_SEARCH;
	d_shift_reg = 0;
	d_preamble_cnt = 0;
	d_chip_cnt = 0;
	d_packet_byte = 0;
}
    
void enter_have_sync()
{
	if (d_verbose)
		fprintf(stderr, "@ enter_have_sync\n");

	d_state = STATE_HAVE_SYNC;
	d_packetlen_cnt = 0;
	d_packet_byte = 0;
	d_packet_byte_index = 0;

	// Link Quality Information
	d_lqi = 0;
	d_lqi_sample_count = 0;
}

void enter_have_header(int payload_len)
{
	if (d_verbose)
		fprintf(stderr, "@ enter_have_header (payload_len = %d)\n", payload_len);

	d_state = STATE_HAVE_HEADER;
	d_packetlen  = payload_len;
	d_payload_cnt = 0;
	d_packet_byte = 0;
	d_packet_byte_index = 0;
}


unsigned char decode_chips(unsigned int chips){
	int i;
	int best_match = 0xFF;
	int min_threshold = 33; // Matching to 32 chips, could never have a error of 33 chips

	for(i=0; i<16; i++) {
		// FIXME: we can store the last chip
		// ignore the first and last chip since it depends on the last chip.
		unsigned int threshold = gr::blocks::count_bits64((chips & MASKTABLE1[d_dsss_index]) ^ (CHIP_MAPPING[d_dsss_index][i] & MASKTABLE1[d_dsss_index]));

		if (threshold < min_threshold) {
			best_match = i;
			min_threshold = threshold;
		}
	}

	if (min_threshold < d_threshold) {
		if (d_verbose)
			fprintf(stderr, "Found sequence with %d errors at 0x%x\n", min_threshold, (chips & MASKTABLE1[d_dsss_index]) ^ (CHIP_MAPPING[d_dsss_index][best_match] & MASKTABLE1[d_dsss_index])), fflush(stderr);
		// LQI: Average number of chips correct * MAX_LQI_SAMPLES
		//
		if (d_lqi_sample_count < MAX_LQI_SAMPLES) {
			d_lqi += 32 - min_threshold;
			d_lqi_sample_count++;
		}

		return (char)best_match & 0xF;
	}

	return 0xFF;
}

int slice(float x) {
	return x > 0 ? 1 : 0;
}

packet_sink_impl(unsigned int threshold,unsigned int dsss_mode,
          int verbose,int verbose2)
  : block ("packet_sink",
		   gr::io_signature::make(1, 1, sizeof(float)),
		   gr::io_signature::make(0, 0, 0)),
    d_threshold(threshold),
    d_dsss_mode(dsss_mode),
    d_verbose(verbose),
    d_verbose2(verbose2)

{
	d_sync_vector = 0xA7;

	// Link Quality Information
	d_lqi = 0;
	d_lqi_sample_count = 0;

  switch(d_dsss_mode) {
    case 8:
      d_dsss_index = 0;
      break;
    case 16:
      d_dsss_index = 1;
      break;
    case 32:
      d_dsss_index = 2;
      break;
    case 64:
      d_dsss_index = 3;
      break;
    default:
      assert(0);
      break;
  }
  

	if ( d_verbose )
		fprintf(stderr, "syncvec: %x, threshold: %d\n", d_sync_vector, d_threshold),fflush(stderr);
	enter_search();

	message_port_register_out(pmt::mp("out"));

}

~packet_sink_impl()
{
}

int general_work(int noutput, gr_vector_int& ninput_items,
			gr_vector_const_void_star& input_items,
			gr_vector_void_star& output_items ) {

	const float *inbuf = (const float*)input_items[0];
        int ninput = ninput_items[0];
	int count=0;
	int i = 0;

	if (d_verbose)
		fprintf(stderr,">>> Entering state machine\n"),fflush(stderr);

	while(count < ninput) {
		switch(d_state) {

		case STATE_SYNC_SEARCH:    // Look for sync vector
			if (d_verbose)
				fprintf(stderr,"SYNC Search, ninput=%d syncvec=%x\n", ninput, d_sync_vector),fflush(stderr);

			while (count < ninput) {

				if(slice(inbuf[count++]))
					d_shift_reg = (d_shift_reg << 1) | 1;
				else
					d_shift_reg = d_shift_reg << 1;

				if(d_preamble_cnt > 0){
					d_chip_cnt = d_chip_cnt+1;
				}

				// The first if block syncronizes to chip sequences.
				if(d_preamble_cnt == 0){
					unsigned int threshold;
					threshold = gr::blocks::count_bits64((d_shift_reg & MASKTABLE1[d_dsss_index]) ^ (CHIP_MAPPING[d_dsss_index][0] & MASKTABLE1[d_dsss_index]));
					if(threshold < d_threshold) {
						//  fprintf(stderr, "Threshold %d d_preamble_cnt: %d\n", threshold, d_preamble_cnt);
						//if ((d_shift_reg&0xFFFFFE) == (CHIP_MAPPING[d_dsss_index][0]&0xFFFFFE)) {
						if (d_verbose2)
							fprintf(stderr,"Found 0 in chip sequence\n"),fflush(stderr);
						// we found a 0 in the chip sequence
						d_preamble_cnt+=1;
						//fprintf(stderr, "Threshold %d d_preamble_cnt: %d\n", threshold, d_preamble_cnt);
					}
				} else {
					// we found the first 0, thus we only have to do the calculation every dsss_mode chips
					if(d_chip_cnt == d_dsss_mode){
						d_chip_cnt = 0;

						if(d_packet_byte == 0) {
							if (gr::blocks::count_bits64((d_shift_reg & MASKTABLE1[d_dsss_index]) ^ (CHIP_MAPPING[d_dsss_index][0] & MASKTABLE2[d_dsss_index])) <= d_threshold) {
								if (d_verbose2)
									fprintf(stderr,"Found %d 0 in chip sequence\n", d_preamble_cnt),fflush(stderr);
								// we found an other 0 in the chip sequence
								d_packet_byte = 0;
								d_preamble_cnt ++;
							} else if (gr::blocks::count_bits64((d_shift_reg & MASKTABLE1[d_dsss_index]) ^ (CHIP_MAPPING[d_dsss_index][7] & MASKTABLE2[d_dsss_index])) <= d_threshold) {
								if (d_verbose2)
									fprintf(stderr,"Found first SFD\n"),fflush(stderr);
								d_packet_byte = 7 << 4;
							} else {
								// we are not in the synchronization header
								if (d_verbose2)
									fprintf(stderr, "Wrong first byte of SFD. %lu\n", d_shift_reg), fflush(stderr);
								enter_search();
								break;
							}

						} else {
							if (gr::blocks::count_bits64((d_shift_reg & MASKTABLE1[d_dsss_index]) ^ (CHIP_MAPPING[d_dsss_index][10] & MASKTABLE2[d_dsss_index])) <= d_threshold) {
								d_packet_byte |= 0xA;
								if (d_verbose2)
									fprintf(stderr,"Found sync, 0x%x\n", d_packet_byte),fflush(stderr);
								// found SDF
								// setup for header decode
								enter_have_sync();
								break;
							} else {
								if (d_verbose)
									fprintf(stderr, "Wrong second byte of SFD. %lu\n", d_shift_reg), fflush(stderr);
								enter_search();
								break;
							}
						}
					}
				}
			}
			break;

		case STATE_HAVE_SYNC:
			if (d_verbose2)
				fprintf(stderr,"Header Search bitcnt=%d, header=0x%08x\n", d_headerbitlen_cnt, d_header),
				fflush(stderr);

			while (count < ninput) {		// Decode the bytes one after another.
				if(slice(inbuf[count++]))
					d_shift_reg = (d_shift_reg << 1) | 1;
				else
					d_shift_reg = d_shift_reg << 1;

				d_chip_cnt = d_chip_cnt+1;

				if(d_chip_cnt == d_dsss_mode){
					d_chip_cnt = 0;
					unsigned char c = decode_chips(d_shift_reg);
					if(c == 0xFF){
						// something is wrong. restart the search for a sync
						if(d_verbose2)
							fprintf(stderr, "Found a not valid chip sequence! %lu\n", d_shift_reg), fflush(stderr);

						enter_search();
						break;
					}

					if(d_packet_byte_index == 0){
						d_packet_byte = c;
					} else {
						// c is always < 15
						d_packet_byte |= c << 4;
					}
					d_packet_byte_index = d_packet_byte_index + 1;
					if(d_packet_byte_index%2 == 0){
						// we have a complete byte which represents the frame length.
						int frame_len = d_packet_byte;
						if(frame_len <= MAX_PKT_LEN){
							enter_have_header(frame_len);
						} else {
							enter_search();
						}
						break;
					}
				}
			}
			break;

		case STATE_HAVE_HEADER:
			if (d_verbose2)
				fprintf(stderr,"Packet Build count=%d, ninput=%d, packet_len=%d\n", count, ninput, d_packetlen),fflush(stderr);

			while (count < ninput) {   // shift bits into bytes of packet one at a time
				if(slice(inbuf[count++]))
					d_shift_reg = (d_shift_reg << 1) | 1;
				else
					d_shift_reg = d_shift_reg << 1;

				d_chip_cnt = (d_chip_cnt+1)%d_dsss_mode;

				if(d_chip_cnt == 0){
					unsigned char c = decode_chips(d_shift_reg);
					if(c == 0xff){
						// something is wrong. restart the search for a sync
						if(d_verbose2)
							fprintf(stderr, "Found a not valid chip sequence! %lu\n", d_shift_reg), fflush(stderr);

						enter_search();
						break;
					}
					// the first symbol represents the first part of the byte.
					if(d_packet_byte_index == 0){
						d_packet_byte = c;
					} else {
						// c is always < 15
						d_packet_byte |= c << 4;
					}
					//fprintf(stderr, "%d: 0x%x\n", d_packet_byte_index, c);
					d_packet_byte_index = d_packet_byte_index + 1;
					if(d_packet_byte_index%2 == 0){
						// we have a complete byte
						if (d_verbose2)
							fprintf(stderr, "packetcnt: %d, payloadcnt: %d, payload 0x%x, d_packet_byte_index: %d\n", d_packetlen_cnt, d_payload_cnt, d_packet_byte, d_packet_byte_index), fflush(stderr);

						d_packet[d_packetlen_cnt++] = d_packet_byte;
						d_payload_cnt++;
						d_packet_byte_index = 0;

						if (d_payload_cnt >= d_packetlen){	// packet is filled, including CRC. might do check later in here
							unsigned int scaled_lqi = (d_lqi / MAX_LQI_SAMPLES) << 3;
							unsigned char lqi = (scaled_lqi >= 256? 255 : scaled_lqi);

							pmt::pmt_t meta = pmt::make_dict();
							meta = pmt::dict_add(meta, pmt::mp("lqi"), pmt::from_long(lqi));

							std::memcpy(buf, d_packet, d_packetlen_cnt);
							pmt::pmt_t payload = pmt::make_blob(buf, d_packetlen_cnt);

							message_port_pub(pmt::mp("out"), pmt::cons(meta, payload));

							if(d_verbose2)
								fprintf(stderr, "Adding message of size %d to queue\n", d_packetlen_cnt);
							enter_search();
							break;
						}
					}
				}
			}
			break;

		default:
			assert(0);
			break;

		}
	}

	if(d_verbose2)
		fprintf(stderr, "Samples Processed: %d\n", ninput_items[0]), fflush(stderr);

        consume(0, ninput_items[0]);

	return 0;
}

private:
	enum {STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER} d_state;

	unsigned int      d_sync_vector;           // 802.15.4 standard is 4x 0 bytes and 1x0xA7
	unsigned int      d_threshold;             // how many bits may be wrong in sync vector

	unsigned long     d_shift_reg;             // used to look for sync_vector
	int               d_preamble_cnt;          // count on where we are in preamble
	int               d_chip_cnt;              // counts the chips collected

	unsigned int      d_header;                // header bits
	int               d_headerbitlen_cnt;      // how many so far

	unsigned char     d_packet[MAX_PKT_LEN];   // assembled payload
	unsigned char     d_packet_byte;           // byte being assembled
	int               d_packet_byte_index;     // which bit of d_packet_byte we're working on
	int               d_packetlen;             // length of packet
	int               d_packetlen_cnt;         // how many so far
	int               d_payload_cnt;           // how many bytes in payload

	unsigned int      d_lqi;                   // Link Quality Information
	unsigned int      d_lqi_sample_count;

  int               d_verbose;                 // very verbose output for almost each sample
  int               d_verbose2;                // less verbose output for higher level debugging
	
  unsigned int      d_dsss_mode;
  int               d_dsss_index;

  // FIXME:
	char buf[256];
};

packet_sink::sptr packet_sink::make(unsigned int threshold,unsigned int dsss_mode,
          int verbose,int verbose2) {
	return gnuradio::get_initial_sptr(new packet_sink_impl(threshold,dsss_mode,verbose,verbose2));
}
