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

function (ADD_CMOCKA_TEST test_name test_src)
  add_executable(${test_name} ${test_src} ${ARGV})
  target_link_libraries(${test_name} ${CMOCKA_LIBRARIES} ${PJSIP_LIBRARIES})
  add_test(${test_name} ${test_name})
  if (USE_MEMTEST)
    add_test (${test_name}_valgrind valgrind
      --error-exitcode=1 --read-var-info=yes
      --leak-check=full --show-leak-kinds=all
      ./${test_name}
      )
  endif (USE_MEMTEST)
endfunction (ADD_CMOCKA_TEST)

add_cmocka_test(test_getopts test_getopts.c
  ${CMAKE_SOURCE_DIR}/src/app_helper/getopts.c)

add_cmocka_test(test_dns_helper test_dns_helper.c
  ${CMAKE_SOURCE_DIR}/src/app_helper/getopts.c
  ${CMAKE_SOURCE_DIR}/src/dns_helper/dns.c
  )
target_link_libraries (test_dns_helper resolv)

