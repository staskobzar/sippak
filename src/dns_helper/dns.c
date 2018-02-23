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
 * @file dns.c
 * @brief sippak DNS helper
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include <pjsip.h>
#include <resolv.h>

#define NAME "dns_helper"
#define MAX_NS_COUNT 3

pj_status_t sippak_set_resolver_ns(pjsip_endpoint *endpt)
{
  pj_status_t status;
  pj_dns_resolver *resv;

  if (res_init() == -1) {
    PJ_LOG(0, (NAME, "failed to init resolv lib"));
    return PJ_ERESOLVE;
  }

  status = pjsip_endpt_create_resolver(endpt, &resv);
  if (status != PJ_SUCCESS) return status;

  /*
  PJ_LOG(1, ("sippak", "Found name servers %d, max allowed server %d",
        _res.nscount, MAX_NS_COUNT));
  for (unsigned i = 0; i < _res.nscount; i++) {
    pj_inet_ntop (pj_AF_INET(), &_res.nsaddr_list[i].sin_addr, addr_str, sizeof(addr_str));
    PJ_LOG(1, ("sippak", "Name server #%d: %s:%d", i + 1, addr_str, htons(_res.nsaddr_list[i].sin_port)));
  }
  pj_str_t nameservers[1];
  nameservers[0] = pj_str("8.8.8.8");
  status = pj_dns_resolver_set_ns(resv, 1, nameservers, NULL);
  return pjsip_endpt_set_resolver(endpt, resv);
  */
}
