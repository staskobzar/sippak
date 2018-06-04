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

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/include/sippak.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/sippak.h"
  @ONLY)

# PJPROJECT compile flags
pkg_check_modules (PJSIP REQUIRED libpjproject>=2.7.1)
#include_directories (${PJSIP_INCLUDE_DIRS})
#link_directories (${PJSIP_LIBRARY_DIRS})
#set(CMAKE_CXX_FLAGS ${PJSIP_CFLAGS_OTHER})

set(EXTRA_LIBS "")
if(APPLE)
  find_library(CORE_AUDIO CoreAudio)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${CORE_AUDIO})
  find_library(CORE_SRV CoreServices)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${CORE_SRV})
  find_library(AUDIO_UNIT AudioUnit)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${AUDIO_UNIT})
  find_library(AUDIO_TOOLBOX AudioToolbox)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${AUDIO_TOOLBOX})
  find_library(FOUNDATION Foundation)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${FOUNDATION})
  find_library(APP_KIT AppKit)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${APP_KIT})
  find_library(AVFOUNDATION AVFoundation)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${AVFOUNDATION})
  find_library(CORE_GRAPH CoreGraphics)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${CORE_GRAPH})
  find_library(QUARTZ_CORE QuartzCore)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${QUARTZ_CORE})
  find_library(CORE_VIDEO CoreVideo)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${CORE_VIDEO})
  find_library(CORE_MEDIA CoreMedia)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${CORE_MEDIA})
  find_library(VID_TOOLBOX VideoToolbox)
  set(EXTRA_LIBS ${EXTRA_LIBS} ${VID_TOOLBOX})
endif(APPLE)

add_subdirectory (mod)
add_subdirectory (app)

add_executable (${PROJECT_NAME} main.c
  $<TARGET_OBJECTS:mod>
  $<TARGET_OBJECTS:app>
  )
target_link_libraries (${PROJECT_NAME} ${PJSIP_LIBRARIES} resolv ${EXTRA_LIBS})

