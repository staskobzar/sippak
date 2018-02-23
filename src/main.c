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
 * @file main.c
 * @brief SIP command line utility main file.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include <pjsip.h>
#include <pjlib.h>

#include "sippak.h"


static pjsip_endpoint *endpt;

int main(int argc, const char *argv[])
{
  pj_status_t status;
  pj_caching_pool pool;
  pj_sockaddr_in addr;
  char addr_str[PJ_INET_ADDRSTRLEN];
  pj_str_t str;
  pjsip_tx_data *tdata;
  pjsip_transport *tp;

  status = pj_init();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

  pj_caching_pool_init(&pool, &pj_pool_factory_default_policy, 0);

  status = pjsip_endpt_create(&pool.factory, "sippak", &endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = sippak_mod_logger_register(endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pj_sockaddr_in_init(&addr, NULL, 0);

  status = sippak_set_resolver_ns (endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_create_request(endpt,
              &pjsip_options_method,  // method OPTIONS
              pj_cstr(&str, "sip:1234@office.modulis.clusterpbx.com"), // request URI
              pj_cstr(&str, "sip:pjsip@localhost"),                    // from header value
              pj_cstr(&str, "sip:1234@office.modulis.clusterpbx.com"), // to header value
              NULL,                   // Contact header
              NULL,                   // Call-ID
              -1,                     // CSeq
              NULL,                   // body
              &tdata);
  pjsip_udp_transport_start( endpt, &addr, NULL, 1, &tp);
  pjsip_endpt_acquire_transport(endpt, PJSIP_TRANSPORT_UDP, &addr, sizeof(addr), NULL, &tp);
  pjsip_endpt_send_request_stateless(endpt, tdata, NULL, NULL);

  for (;;)
    pjsip_endpt_handle_events(endpt, NULL);

  pj_caching_pool_destroy(&pool);

  return 0;
}
