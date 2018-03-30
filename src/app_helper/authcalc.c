/**
 * sippak -- SIP command line utility.
 * Copyright (C) 2018, Stas Kobzar <staskobzar@modulis.ca>
 *
 * This file is part of sippak.
 *
 * sippak is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sippak is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sippak.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file authcalc.c
 * @brief sippak helper to parse and calculate from WWW/Proxy Authenticate header
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include "sippak.h"

pj_status_t sippak_auth_calc (struct sippak_app *app)
{
  pj_status_t status = PJ_SUCCESS;
  char result_buf[PJSIP_MD5STRLEN + 1];
  pj_str_t result;

  result.slen = PJSIP_MD5STRLEN + 1;
  result.ptr = result_buf;

  return status;
}
