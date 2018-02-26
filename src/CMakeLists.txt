#
# sippak -- SIP command line utility.
# Copyright (C) 2018, Stas Kobzar <staskobzar@modulis.ca>
#
# This file is part of sippak.
#
# sippack is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# sippack is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with sippack.  If not, see <http://www.gnu.org/licenses/>.
#

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include )

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/sippak.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/sippak.h"
  @ONLY)

# PJPROJECT compile flags
include (FindPkgConfig)
pkg_check_modules (PJSIP REQUIRED libpjproject>=2.7.1)
include_directories (${PJSIP_INCLUDE_DIRS})
link_directories (${PJSIP_LIBRARY_DIRS})
set(CMAKE_CXX_FLAGS ${PJSIP_CFLAGS_OTHER})

add_subdirectory (mod_logger)
add_subdirectory (mod_ping)
add_subdirectory (dns_helper)
add_subdirectory (app_helper)

add_executable (${PROJECT_NAME} main.c
  $<TARGET_OBJECTS:mod_logger>
  $<TARGET_OBJECTS:mod_ping>
  $<TARGET_OBJECTS:dns_helper>
  $<TARGET_OBJECTS:app_helper>
  )
target_link_libraries (${PROJECT_NAME} ${PJSIP_LIBRARIES} resolv)

