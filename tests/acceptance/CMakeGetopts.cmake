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
add_test (PrintVersion ${EXECMD} -V )
set_tests_properties (PrintVersion PROPERTIES PASS_REGULAR_EXPRESSION
  "${PROJECT_NAME} ${PROJECT_VERSION}")
set_tests_properties (PrintVersion PROPERTIES FAIL_REGULAR_EXPRESSION
  "Usage: ")

# print usage when no arguments
add_test (NoArgsUsage ${EXECMD})
set_tests_properties (NoArgsUsage PROPERTIES PASS_REGULAR_EXPRESSION
  "Usage: ${PROJECT_NAME}")

# print usage short argument
add_test (ShortOptHelpUsage ${EXECMD} -h)
set_tests_properties (ShortOptHelpUsage PROPERTIES PASS_REGULAR_EXPRESSION
  "Usage: ${PROJECT_NAME}")

# print usage long argument
add_test (LongOptHelpUsage ${EXECMD} --help)
set_tests_properties (LongOptHelpUsage PROPERTIES PASS_REGULAR_EXPRESSION
  "Usage: ${PROJECT_NAME}")


