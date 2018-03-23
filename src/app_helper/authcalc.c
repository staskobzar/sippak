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
  pj_status_t status;
  /*
  pj_ssize_t found_idx = 0;
  pj_str_t token;
  pj_str_t hname;
  pj_str_t delim = pj_str(":");

  found_idx = pj_strtok (&app->cfg.auth_header, &delim, &token, 0);
  */

  pjsip_hdr *hdr;
  pjsip_hdr *hlist = PJ_POOL_ZALLOC_T(app->pool, pjsip_hdr);
  pj_list_init(hlist);

  status = pjsip_parse_headers(app->pool,
      app->cfg.auth_header.ptr,
      app->cfg.auth_header.slen,
      hlist, 0);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  hdr = hlist->next;

  if (hdr->type == PJSIP_H_WWW_AUTHENTICATE) {
    printf("=============> header type: %d, name %.*s\n", hdr->type,
        hdr->name.slen, hdr->name.ptr);
    const pjsip_www_authenticate_hdr *hchal;
    hchal = (const pjsip_www_authenticate_hdr*)hdr;
    printf("=============> realm: %.*s\n",
        hchal->challenge.common.realm.slen,
        hchal->challenge.common.realm.ptr);
/*
 // * Build digest credential from arguments.


692     if (hdr->type == PJSIP_H_WWW_AUTHENTICATE)
 693   hauth = pjsip_authorization_hdr_create(pool);
 694     else if (hdr->type == PJSIP_H_PROXY_AUTHENTICATE)
 695   hauth = pjsip_proxy_authorization_hdr_create(pool);
 696     else {
 697   return PJSIP_EINVALIDHDR;
 698     }

pjsip_authorization_hdr *hauth;

cred = hauth->credential.digest
pjsip_digest_credential *cred;
pjsip_cred_info *cred_info
pjsip_digest_challenge *chal

 294     pj_strdup(pool, &cred->username, &cred_info->username);
 295     pj_strdup(pool, &cred->realm, &chal->realm);
 296     pj_strdup(pool, &cred->nonce, &chal->nonce);
 297     pj_strdup(pool, &cred->uri, uri);
 298     pj_strdup(pool, &cred->algorithm, &chal->algorithm);
 299     pj_strdup(pool, &cred->opaque, &chal->opaque);

*/
  }

  /*
  pj_strcpy(&hname, &token);
  printf("found_idx : %d, %.*s\n", found_idx, hname.slen, hname.ptr);
  found_idx = pj_strtok (&app->cfg.auth_header, &delim, &token, found_idx + token.slen);
  printf("found_idx : %d, %.*s\n", found_idx, token.slen, token.ptr);

  char *val = "Digest realm=\"testrealm@host.com\",qop=\"auth,auth-int\",nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\",opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"";
  int len;
  pj_str_t dst;
  pj_strdup_with_null (app->pool, &dst, &token);
  printf("----> after pool: %.*s\n", dst.slen, dst.ptr);

  pjsip_generic_string_hdr *hstruct = pjsip_parse_hdr(app->pool, &hdr, val, strlen(val), NULL);
  printf("-----=====----> hstruct->type = %d\n", hstruct->type);
  */
  /*

  pjsip_route_hdr *route;
  const pj_str_t hn = { "Route", 5 };
  char *uri = "sip:proxy.server;lr";
  route = pjsip_parse_hdr( app->pool, &hn, uri, strlen(uri), NULL);
  */

  return PJ_SUCCESS;
}
