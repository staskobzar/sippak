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

cmake_minimum_required (VERSION 3.5)

project (sippak VERSION 1.0.0 LANGUAGES C)
set (PROJECT_URL "https://gitlab.com/staskobzar/sippak")
set (PROJECT_AUTHOR "Stas Kobzar <staskobzar@gmail.com>")
string (TIMESTAMP PROJECT_BUILTTIME "%Y-%m-%d %H:%M:%SZ UTC" UTC)

include (FindPkgConfig)

set(EXTRA_LIBS "")
# MacOS frameworks to use with pjmedia
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

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/src/include )
add_subdirectory (src)

enable_testing()
add_subdirectory (tests)
