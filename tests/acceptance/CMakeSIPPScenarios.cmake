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

set (SCENARIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sipp_scenarios)

# basic OPTIONS request send
execute_process (
  COMMAND ${SIPP} -timeout 10s -m 1 -bg -sf ${SCENARIO_DIR}/options.basic.xml
  TIMEOUT 10
  )
add_test (
  OPTIONS_Basic_Request
  ${EXECMD} sip:alice@127.0.0.1
  )
set_tests_properties ( OPTIONS_Basic_Request
  PROPERTIES PASS_REGULAR_EXPRESSION "OK Basic OPTIONS")

