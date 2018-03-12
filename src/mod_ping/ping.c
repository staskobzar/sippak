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
  PJSIP_MOD_PRIORITY_TSX_LAYER, /* Priority          */
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

/* Create From SIP header */
static pj_str_t sippak_create_from(struct sippak_app *app)
{
  pj_str_t from=app->cfg.dest;
  pjsip_sip_uri *dest_uri = (pjsip_sip_uri*)pjsip_parse_uri(app->pool, app->cfg.dest.ptr,
                          app->cfg.dest.slen, 0);

  printf("=====> host = %.*s\n", dest_uri->host.slen, dest_uri->host.ptr);

  return from;
}

/* Create Contact SIP header */
static pj_str_t sippak_create_contact(struct sippak_app *app,
                                      pj_sockaddr_in *addr)
{
  pj_str_t cnt = {0,0};
  char addr_str[128];
  char contact[128];

  pj_sockaddr_print(addr, addr_str, 128, 1);
  pj_ansi_sprintf(contact, "sip:%.*s@%s",
      (int)app->cfg.username.slen, app->cfg.username.ptr,
      addr_str);

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

  PJ_LOG(3, (NAME, "Response received: %d %.*s",
        msg->line.status.code,
        msg->line.status.reason.slen,
        msg->line.status.reason.ptr));

  sippak_loop_cancel();

  return PJ_FALSE; // continue with othe modules
}

/* Ping */
pj_status_t sippak_cmd_ping (struct sippak_app *app)
{
  pj_status_t status;
  pj_str_t str;
  pj_sockaddr_in addr;
  pjsip_transport *tp;
  pjsip_tx_data *tdata;
  pjsip_transaction *tsx;

  pj_str_t cnt, from;

  status = pj_sockaddr_in_init(&addr, NULL, app->cfg.local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_udp_transport_start( app->endpt, &addr, NULL, 1, &tp);
  if (status != PJ_SUCCESS) {
    char addr_str[128];
    PJ_LOG(1, (NAME, "Failed to init local address %s.",
          pj_sockaddr_print(&addr, addr_str, 128, 1)));
    return status;
  }

  cnt = sippak_create_contact(app, &addr);
  from = sippak_create_from(app);

  status = pjsip_endpt_acquire_transport(app->endpt, PJSIP_TRANSPORT_UDP, &addr, sizeof(addr), NULL, &tp);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_create_request(app->endpt,
              &pjsip_options_method,        // method OPTIONS
              &app->cfg.dest, // request URI
              &from, // from header value
              &app->cfg.dest, // to header value
              &cnt,          // Contact header
              NULL,          // Call-ID
              -1,            // CSeq
              NULL,          // body
              &tdata);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_create_uac(&mod_ping, tdata, &tsx);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  return pjsip_tsx_send_msg(tsx, tdata);
}
