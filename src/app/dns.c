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
#include <resolv.h>
#include "sippak.h"

#define NAME "dns_helper"

/* get port as int from server string. token ":". */
static int get_port (pj_str_t server)
{
  pj_ssize_t found_idx = 0;
  pj_str_t token;
  int port = 53; // default DNS port is 53
  pj_str_t delim = pj_str(":");

  found_idx = pj_strtok (&server, &delim, &token, 0);
  found_idx = pj_strtok (&server, &delim, &token, found_idx + token.slen);

  if (found_idx == server.slen) {
    // port in server string is not found
    return port; // return default port
  }

  port = pj_strtol (&token);
  port = port == 0 ? 53 : port;

  return port;
}

/* get server ip from pj_str without port if exists. */
static pj_str_t get_server_ip (pj_str_t token)
{
  pj_str_t ip_str;
  int idx = 0;
  char *ptr = token.ptr;

  for (; idx < token.slen && ptr[idx] != ':'; idx++);

  ip_str.ptr = token.ptr;
  ip_str.slen = idx;
  return ip_str;
}

static int parse_nameservers_string (char *nameservers,
                                     pj_str_t *ns,
                                     pj_uint16_t *ports)
{
  pj_ssize_t found_idx = 0;
  pj_str_t token;
  int ns_idx = 0;
  pj_str_t ns_str= pj_str(nameservers);
  pj_str_t delim = pj_str(",");

  for (
      found_idx = pj_strtok (&ns_str, &delim, &token, 0);
      found_idx != ns_str.slen;
      found_idx = pj_strtok (&ns_str, &delim, &token, found_idx + token.slen)
      )
  {
    if (ns_idx >= MAX_NS_COUNT) {
      PJ_LOG(4, (NAME, "More NS servers then allowed. Skip server %.*s", token.slen, token.ptr));
      continue;
    }
    ns[ns_idx] = get_server_ip(token);
    ports[ns_idx] = get_port(token);
    ns_idx++;
  }

  return ns_idx;
}

/* when nameservers are not given, try to get from system resolv.conf */
static int system_resolv_ns ( pj_pool_t *pool,
                              pj_str_t *ns,
                              pj_uint16_t *ports)
{
  int ns_idx = 0;
  int loop_ns = 0;
  char addr_str[PJ_INET_ADDRSTRLEN];

  if (res_init() == -1) {
    PJ_LOG(1, (NAME, "Failed to init resolv lib. DNS is disabled."));
    return 0;
  }

  PJ_LOG(4, (NAME, "Found %d name servers configured on system. Will use first IPv4 one.",
        _res.nscount, MAX_NS_COUNT));

  loop_ns = _res.nscount > MAX_NS_COUNT ? MAX_NS_COUNT : _res.nscount;

  for (unsigned i = 0; ns_idx < loop_ns && i < _res.nscount ; i++ ) {
    if (_res.nsaddr_list[i].sin_family != pj_AF_INET()) {
        PJ_LOG(4, (NAME, "Skip non IPv4 name server"));
        continue;
    }

    pj_inet_ntop (pj_AF_INET(), &_res.nsaddr_list[i].sin_addr, addr_str, sizeof(addr_str));

    ns[ns_idx] = pj_strdup3(pool, addr_str);
    ports[ns_idx] = pj_ntohs(_res.nsaddr_list[i].sin_port);

    ns_idx++;
  }

  return ns_idx;
}

PJ_DEF(int) sippak_get_ns_list (struct sippak_app *app,
                        pj_str_t *ns,
                        pj_uint16_t *ports)
{
  if (app->cfg.nameservers == NULL) {

    return system_resolv_ns (app->pool, ns, ports);

  } else {

    return parse_nameservers_string (app->cfg.nameservers, ns, ports);

  }
}

PJ_DEF(pj_status_t) sippak_set_resolver_ns(struct sippak_app *app)
{
  pj_status_t status;
  pj_dns_resolver *resv;
  pj_str_t nameservers[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  unsigned serv_num = 0;

  serv_num = sippak_get_ns_list (app, nameservers, ports);

  status = pjsip_endpt_create_resolver(app->endpt, &resv);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pj_dns_resolver_set_ns(resv, serv_num, nameservers, ports);
  if (status != PJ_SUCCESS) return status;

  return pjsip_endpt_set_resolver(app->endpt, resv);
}
