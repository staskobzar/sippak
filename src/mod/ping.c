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
 * @brief sippak ping with OPTIONS remote host. Stateless.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjsip.h>
#include <pjlib.h>
#include "sippak.h"

#define NAME "mod_ping"

static pjsip_auth_clt_sess auth_sess;
static int auth_tries = 0;

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);

static pjsip_module mod_ping =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-ping", 9 },          /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_TRANSPORT_LAYER, /* Priority          */
  NULL,                       /* load()    */
  NULL,                       /* start()    */
  NULL,                       /* stop()    */
  NULL,                       /* unload()    */
  NULL,                       /* on_rx_request()  */
  &on_rx_response,            /* on_rx_response()  */
  NULL,                       /* on_tx_request.  */
  NULL,                       /* on_tx_response()  */
  NULL,                       /* on_tsx_state()  */
};

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

  int code = rdata->msg_info.msg->line.status.code;

  if (code == 401 || code == 407) {
    return PJ_FALSE; // processed with callback
  }

  sippak_loop_cancel();

  return PJ_FALSE; // continue with othe modules
}

static void send_cb(void *token, pjsip_event *e)
{
  pj_status_t status;
  pjsip_tx_data *tdata;
  pjsip_cred_info	cred[1];
  pjsip_transaction *tsx = e->body.tsx_state.tsx;
  pjsip_rx_data *rdata = e->body.tsx_state.src.rdata;
  struct sippak_app *app = token;
  if (tsx->status_code == 401 || tsx->status_code == 407) {
    auth_tries++;
    if (auth_tries > 1) {
      PJ_LOG(1, (NAME, "Authentication failed. Check your username and password"));
      sippak_loop_cancel();
      return;
    }
    sippak_set_cred(app, cred);

    status = pjsip_auth_clt_init (&auth_sess, app->endpt, rdata->tp_info.pool, 0);
    if (status != PJ_SUCCESS) {
      PJ_LOG(1, (NAME, "Failed init authentication credentials."));
      return;
    }

    pjsip_auth_clt_set_credentials(&auth_sess, 1, cred);

    status = pjsip_auth_clt_reinit_req(&auth_sess, rdata,
        tsx->last_tx, &tdata);
    if (status != PJ_SUCCESS) {
      PJ_LOG(1, (NAME, "Failed to re-init client authentication session."));
      return;
    }

    pjsip_endpt_send_request(app->endpt, tdata, -1, app, &send_cb);
  }
}

/* Ping */
pj_status_t sippak_cmd_ping (struct sippak_app *app)
{
  pj_status_t status;
  pj_str_t *local_addr;
  int local_port;

  pjsip_tx_data *tdata = NULL;

  pj_str_t cnt, from, ruri;

  status = sippak_transport_init(app, &local_addr, &local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  cnt  = sippak_create_contact_hdr(app, local_addr, local_port);
  from = sippak_create_from_hdr(app);
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

  status = pjsip_endpt_register_module(app->endpt, &mod_ping);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  sippak_add_sip_headers (tdata, app);

  return pjsip_endpt_send_request(app->endpt, tdata, -1, app, &send_cb);
}
