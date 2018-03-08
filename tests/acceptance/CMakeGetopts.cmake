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

# output version and exit
add_test (Print_Version ${EXECMD} -V )
set_tests_properties (Print_Version PROPERTIES PASS_REGULAR_EXPRESSION
  "${PROJECT_NAME} ${PROJECT_VERSION}")
set_tests_properties (Print_Version PROPERTIES FAIL_REGULAR_EXPRESSION
  "Usage: ")

# print usage when no arguments
add_test (NoArgs_Usage ${EXECMD})
set_tests_properties (NoArgs_Usage PROPERTIES PASS_REGULAR_EXPRESSION
  "Usage: ${PROJECT_NAME}")

# print usage short argument
add_test (Short_Opt_Help_Usage ${EXECMD} -h)
set_tests_properties (Short_Opt_Help_Usage PROPERTIES PASS_REGULAR_EXPRESSION
  "Usage: ${PROJECT_NAME}")

# print usage long argument
add_test (Long_Opt_Help_Usage ${EXECMD} --help)
set_tests_properties (Long_Opt_Help_Usage PROPERTIES PASS_REGULAR_EXPRESSION
  "Usage: ${PROJECT_NAME}")

# usage has all options
add_test (Usage_Has_All_Opts
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.usage_opts.pl
  ${CMAKE_SOURCE_DIR}/src/app_helper/getopts.c ${EXECMD})

