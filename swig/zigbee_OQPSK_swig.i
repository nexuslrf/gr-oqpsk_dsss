/* -*- c++ -*- */

#define ZIGBEE_OQPSK_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "zigbee_OQPSK_swig_doc.i"

%{
#include "zigbee_OQPSK/access_code_prefixer.h"
%}

%include "zigbee_OQPSK/access_code_prefixer.h"
GR_SWIG_BLOCK_MAGIC2(zigbee_OQPSK, access_code_prefixer);
