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
#

########################################################################
# Include swig generation macros
########################################################################
include(GrPython)
find_package(SWIG)
find_package(PythonLibs)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)

########################################################################
# Setup swig generation
########################################################################
set(GR_SWIG_INCLUDE_DIRS
    ${GR_OQPSK_DSSS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

foreach(incdir ${GNURADIO_RUNTIME_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/gnuradio/swig)
endforeach(incdir)

set(GR_SWIG_LIBRARIES gnuradio-oqpsk_dsss)
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/oqpsk_dsss_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)

GR_SWIG_MAKE(oqpsk_dsss_swig oqpsk_dsss_swig.i)

########################################################################
# Install the build swig module
########################################################################
GR_SWIG_INSTALL(
    TARGETS oqpsk_dsss_swig
    DESTINATION ${GR_PYTHON_DIR}/oqpsk_dsss
    COMPONENT "oqpsk_dsss_python"
)

########################################################################
# Install swig .i files for development
########################################################################
list(APPEND swig_sources
    oqpsk_dsss_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/oqpsk_dsss_swig_doc.i
)

install(
    FILES ${swig_sources}
    DESTINATION ${GR_INCLUDE_DIR}/oqpsk_dsss/swig
    COMPONENT "oqpsk_dsss_swig"
)
