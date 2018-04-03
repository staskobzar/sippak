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
 * @file ping.c
 * @brief sippak ping with OPTIONS remote host
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjsip.h>
#include <pjlib.h>
#include "sippak.h"

#define NAME "mod_ping"

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);
static void on_tsx_state(pjsip_transaction *tsx, pjsip_event *event);

static pjsip_module mod_ping =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-ping", 9 },          /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_DIALOG_USAGE - 1, /* Priority          */
  NULL,                       /* load()    */
  NULL,                       /* start()    */
  NULL,                       /* stop()    */
  NULL,                       /* unload()    */
  NULL,                       /* on_rx_request()  */
  &on_rx_response,            /* on_rx_response()  */
  NULL,                       /* on_tx_request.  */
  NULL,                       /* on_tx_response()  */
  &on_tsx_state,              /* on_tsx_state()  */
};

/* Create Requst-URI */
static pj_str_t sippak_create_ruri(struct sippak_app *app)
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

/* Create From SIP header */
static pj_str_t sippak_create_from(struct sippak_app *app)
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

/* Create Contact SIP header */
static pj_str_t sippak_create_contact (struct sippak_app *app,
                                      pj_str_t local_addr,
                                      int local_port)
{
  pj_str_t cnt = {0,0};
  char contact[PJSIP_MAX_URL_SIZE];

  pj_ansi_sprintf(contact, "sip:%.*s@%.*s:%d",
      (int)app->cfg.username.slen, app->cfg.username.ptr,
      (int)local_addr.slen, local_addr.ptr, local_port);

  cnt = pj_strdup3(app->pool, contact);

  return cnt;
}

/* On transaction state callback */
static void on_tsx_state(pjsip_transaction *tsx, pjsip_event *event)
{
  if( tsx->status_code == 408 && tsx->state == PJSIP_TSX_STATE_TERMINATED ) {
    PJ_LOG(3, (NAME, "Response received: %d %.*s.",
          tsx->status_code, tsx->status_text.slen, tsx->status_text.ptr));
    PJ_LOG(3, (NAME, "Retransmitions count: %d.", tsx->retransmit_count));

    sippak_loop_cancel();
  }
}

/* On response module callback */
static pj_bool_t on_rx_response (pjsip_rx_data *rdata)
{
  pjsip_msg *msg = rdata->msg_info.msg;
  pjsip_via_hdr *via = rdata->msg_info.via;

  PJ_LOG(3, (NAME, "Response received: %d %.*s",
        msg->line.status.code,
        msg->line.status.reason.slen,
        msg->line.status.reason.ptr));
  if (via) {
    PJ_LOG(3, (NAME, "Via rport: %d, received: %.*s", via->rport_param,
          via->recvd_param.slen, via->recvd_param.ptr));
  }

  sippak_loop_cancel();

  return PJ_FALSE; // continue with othe modules
}

/* Ping */
pj_status_t sippak_cmd_ping (struct sippak_app *app)
{
  pj_status_t status;
  pj_sockaddr_in addr;
  pj_str_t local_addr = {0, 0};
  int local_port = 0;

  pjsip_transport *tp = NULL;
  pjsip_tpfactory *tpfactory = NULL;

  pjsip_tx_data *tdata = NULL;
  pjsip_transaction *tsx = NULL;

  pj_str_t cnt, from, ruri;

  status = pj_sockaddr_in_init(&addr, &app->cfg.local_host, app->cfg.local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  // set transport TCP/UDP
  if (app->cfg.proto == PJSIP_TRANSPORT_TCP) {

    status = pjsip_tcp_transport_start( app->endpt, &addr, 1, &tpfactory);

    if (status == PJ_SUCCESS) {
      local_addr = tpfactory->addr_name.host;
      local_port = tpfactory->addr_name.port;
    }

  } else {

    status = pjsip_udp_transport_start( app->endpt, &addr, NULL, 1, &tp);

    if (status == PJ_SUCCESS) {
      local_addr = tp->local_name.host;
      local_port = tp->local_name.port;
    }
  }

  if (status != PJ_SUCCESS) {
    char addr_str[PJSIP_MAX_URL_SIZE];
    PJ_LOG(1, (NAME, "Failed to init local address %s.",
          pj_sockaddr_print(&addr, addr_str, PJSIP_MAX_URL_SIZE, 1)));
    return status;
  }

  cnt = sippak_create_contact(app, local_addr, local_port);
  from = sippak_create_from(app);
  ruri = sippak_create_ruri(app);

  status = pjsip_endpt_create_request(app->endpt,
              &pjsip_options_method,  // method OPTIONS
              &ruri,                  // request URI
              &from,                  // from header value
              &app->cfg.dest,         // to header value
              &cnt,                   // Contact header
              NULL,                   // Call-ID
              -1,                     // CSeq
              NULL,                   // body
              &tdata);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_create_uac(&mod_ping, tdata, &tsx);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  return pjsip_tsx_send_msg(tsx, tdata);
}
