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

set (SIPP_SCENARIO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/sipp_scenarios)

add_test ( Ping_With_OPTIONS
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.ping_options.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( Ping_over_TCP
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.ping_options_tcp.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

option (TEST_TIMEOUT "Test timeout SIP. Takes around 30 sec." OFF)
if (TEST_TIMEOUT)
  add_test ( Ping_Timeoute_Retrans
    perl ${CMAKE_CURRENT_SOURCE_DIR}/test.ping_retrans.pl
    ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})
endif(TEST_TIMEOUT)

add_test ( Ping_Source_Port_Username
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.ping_src_port_host.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( Ping_Proxy_Auth
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.ping_auth.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( PUBLISH_method
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.publish_basic.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( PUBLISH_auth
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.publish_auth.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( SUBSCRIBE_method
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.subscribe_basic.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( SUBSCRIBE_terminate_subscription
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.subscribe_terminate.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( SUBSCRIBE_auth_request
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.subscribe_auth.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( SUBSCRIBE_MWI_request
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.subscribe_mwi.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

add_test ( NOTIFY_method_send
  perl ${CMAKE_CURRENT_SOURCE_DIR}/test.notify_basic.pl
  ${EXECMD} ${SIPP} ${SIPP_SCENARIO_PATH})

