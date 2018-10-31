#
# Copyright 2013 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

#####################################################################
# Setup library
#####################################################################
include(GrPlatform) #define LIB_SUFFIX
list(APPEND oqpsk_dsss_sources
    access_code_prefixer.cc
    packet_sink.cc
)

list(APPEND oqpsk_dsss_libs
    ${Boost_LIBRARIES}
    ${GNURADIO_ALL_LIBRARIES}
    ${VOLK_LIBRARIES}
)

add_library(gnuradio-oqpsk_dsss SHARED ${oqpsk_dsss_sources})
target_link_libraries(gnuradio-oqpsk_dsss ${oqpsk_dsss_libs})
set_target_properties(gnuradio-oqpsk_dsss PROPERTIES DEFINE_SYMBOL "gnuradio_oqpsk_dsss_EXPORTS")

#####################################################################
# Install built library files
#####################################################################
install(TARGETS gnuradio-oqpsk_dsss
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

