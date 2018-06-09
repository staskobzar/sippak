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
 * @file sip_helper.c
 * @brief sippak ping with OPTIONS remote host
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "sippak.h"

#define NAME "sip_helper"

/* Create From SIP header */
PJ_DEF(pj_str_t) sippak_create_from_hdr(struct sippak_app *app)
{
  pj_str_t from = {0,0};
  from.ptr = (char*)pj_pool_alloc(app->pool, PJSIP_MAX_URL_SIZE);
  pjsip_sip_uri *dest_uri = (pjsip_sip_uri*)pjsip_parse_uri(app->pool, app->cfg.dest.ptr,
                          app->cfg.dest.slen, 0);
  if (dest_uri == NULL) {
    PJ_LOG(1, (NAME, "Failed to parse URI %s for From header.", app->cfg.dest));
    return app->cfg.dest;
  }

  dest_uri->user = app->cfg.username;
  from.slen = pjsip_uri_print(PJSIP_URI_IN_FROMTO_HDR, dest_uri, from.ptr, PJSIP_MAX_URL_SIZE);

  if (from.slen == -1) {
    PJ_LOG(1, (NAME, "Failed to print from uri to buffer."));
    return app->cfg.dest;
  }

  if (app->cfg.from_name.ptr) {
    pjsip_name_addr *name_uri = pjsip_name_addr_create(app->pool);
    name_uri->uri = (pjsip_uri*) dest_uri;
    name_uri->display = app->cfg.from_name;

    from.slen = pjsip_uri_print(PJSIP_URI_IN_FROMTO_HDR, name_uri, from.ptr, PJSIP_MAX_URL_SIZE);
  }

  return from;
}

/* Create Requst-URI */
PJ_DEF(pj_str_t) sippak_create_ruri(struct sippak_app *app)
{
  pj_str_t ruri = {0,0};
  ruri.ptr = (char*)pj_pool_alloc(app->pool, PJSIP_MAX_URL_SIZE);
  pjsip_sip_uri *dest_uri = (pjsip_sip_uri*)pjsip_parse_uri(app->pool, app->cfg.dest.ptr,
                          app->cfg.dest.slen, 0);
  if (dest_uri == NULL) {
    PJ_LOG(1, (NAME, "Failed to parse URI %s for Request-URI.", app->cfg.dest));
    return app->cfg.dest;
  }

  if (app->cfg.proto == PJSIP_TRANSPORT_TCP) {
    dest_uri->transport_param = pj_str("tcp");
  }

  ruri.slen = pjsip_uri_print(PJSIP_URI_IN_REQ_URI, dest_uri, ruri.ptr, PJSIP_MAX_URL_SIZE);
  if (ruri.slen == -1) {
    PJ_LOG(1, (NAME, "Failed to print Request-URI to buffer."));
    return app->cfg.dest;
  }

  return ruri;
}

/* Create REGISTER Requst-URI */
PJ_DEF(pj_str_t) sippak_create_reg_ruri(struct sippak_app *app)
{
  pj_str_t reg_ruri = {0,0};
  reg_ruri.ptr = (char*)pj_pool_alloc(app->pool, PJSIP_MAX_URL_SIZE);
  pj_str_t ruri = sippak_create_ruri(app);

  pjsip_sip_uri *ruri_s = (pjsip_sip_uri*)pjsip_parse_uri(app->pool,
      ruri.ptr, ruri.slen, 0);

  // remove user part
  ruri_s->user.slen = 0;
  ruri_s->user.ptr = NULL;

  reg_ruri.slen = pjsip_uri_print(PJSIP_URI_IN_REQ_URI, ruri_s,
      reg_ruri.ptr, PJSIP_MAX_URL_SIZE);

  return reg_ruri;
}

/* Create Contact SIP header */
PJ_DEF(pj_str_t) sippak_create_contact_hdr (struct sippak_app *app,
                                pj_str_t *local_addr,
                                int local_port)
{
  pj_str_t cnt = {0,0};
  char contact[PJSIP_MAX_URL_SIZE];

  if (app->cfg.contact.slen > 0) {
    return app->cfg.contact;
  }

  pj_ansi_sprintf(contact, "sip:%.*s@%.*s:%d",
      (int)app->cfg.username.slen, app->cfg.username.ptr,
      (int)local_addr->slen, local_addr->ptr, local_port);

  cnt = pj_strdup3(app->pool, contact);

  return cnt;
}

/* Init transport */
PJ_DEF(pj_status_t) sippak_transport_init(struct sippak_app *app,
                                  pj_str_t **local_addr,
                                  int *local_port)
{
  pj_status_t status;
  pj_sockaddr_in addr;
  pjsip_transport *tp = NULL;
  pjsip_tpfactory *tpfactory = NULL;

  status = pj_sockaddr_in_init(&addr, &app->cfg.local_host, app->cfg.local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  // set transport TCP/UDP
  if (app->cfg.proto == PJSIP_TRANSPORT_TCP) {

    status = pjsip_tcp_transport_start( app->endpt, &addr, 1, &tpfactory);

    if (status == PJ_SUCCESS) {
      *local_addr = &tpfactory->addr_name.host;
      *local_port = tpfactory->addr_name.port;
    }

  } else {

    status = pjsip_udp_transport_start( app->endpt, &addr, NULL, 1, &tp);

    if (status == PJ_SUCCESS) {
      *local_addr = &tp->local_name.host;
      *local_port = tp->local_name.port;
    }
  }

  if (status != PJ_SUCCESS) {
    char addr_str[PJSIP_MAX_URL_SIZE];
    PJ_LOG(1, (NAME, "Failed to init local address %s.",
          pj_sockaddr_print(&addr, addr_str, PJSIP_MAX_URL_SIZE, 1)));
  }

  return status;
}

PJ_DEF(void) sippak_set_cred(struct sippak_app *app,
                            pjsip_cred_info *cred)
{
  cred->realm     = pj_str("*");
  cred->scheme    = pj_str("digest");
  cred->username  = app->cfg.username;
  cred->data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  cred->data      = app->cfg.password;
}

PJ_DEF(void) sippak_add_sip_headers (pjsip_tx_data *tdata, struct sippak_app *app)
{
  pj_str_t ua_hname = pj_str("User-Agent");
  pjsip_hdr *hdr;

  if(pjsip_msg_find_hdr_by_name(tdata->msg, &ua_hname, NULL) == NULL
      && app->cfg.user_agent.slen > 0) {
    hdr = (pjsip_hdr*) pjsip_generic_string_hdr_create(tdata->pool,
        &ua_hname, &app->cfg.user_agent);

    pjsip_msg_add_hdr(tdata->msg, hdr);
  }

  if (app->cfg.hdrs.cnt > 0) {
    for(int i = 0; i < app->cfg.hdrs.cnt; i++) {
      pjsip_hdr *hdr_add = (pjsip_hdr *)app->cfg.hdrs.h[i];
      hdr = pjsip_msg_find_hdr_by_name(tdata->msg, &hdr_add->name, NULL);
      if(hdr == NULL) {
        pjsip_msg_add_hdr(tdata->msg, hdr_add);
      }
    }
  }
}
