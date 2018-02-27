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

include_directories( ${CMAKE_CURRENT_BINARY_DIR}/src/include )
add_subdirectory (src)

enable_testing()
add_subdirectory (tests)
