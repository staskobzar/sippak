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

# create distribution packages

SET(SIPPAK_PKG_VER "${sippak_VERSION}-beta")

set (CPACK_GENERATOR "RPM;DEB;TGZ")
set (CPACK_PACKAGE_VERSION ${SIPPAK_PKG_VER})
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "SIP protocol command line utility.")

set (CPACK_DEBIAN_PACKAGE_MAINTAINER "Stas Kobzar <staskobzar@gmail.com>")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libalsaplayer0,libasound2")
set (CPACK_DEBIAN_PACKAGE_SECTION "net")

set (CPACK_RPM_PACKAGE_VENDOR "Stas Kobzar <staskobzar@gmail.com>")
set (CPACK_RPM_PACKAGE_REQUIRES "alsa-lib>=1.1.4")
set (CPACK_RPM_PACKAGE_LICENSE "GNU General Public License v3.0")
set (CPACK_RPM_PACKAGE_GROUP "Networking Tools")

include (CPack)
