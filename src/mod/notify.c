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
 * @brief sippak send stateless NOTIFY message
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjsip.h>
#include <pjlib.h>
#include "sippak.h"
#include <pjsip-simple/presence.h> // decare notify method

#define NAME "mod_notify"

static pjsip_auth_clt_sess auth_sess;
static int auth_tries = 0;

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);

static pjsip_module mod_notify =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-notify", 10 },       /* Name.    */
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

static void add_event_hdr(pjsip_tx_data *tdata, struct sippak_app *app)
{
  pj_str_t hname = pj_str("Event");
  pj_str_t hvalue;
  pjsip_generic_string_hdr *event_hdr;

  if ((app->cfg.ctype_e == CTYPE_PIDF || app->cfg.ctype_e == CTYPE_XPIDF)
      && app->cfg.pres_ev == EVTYPE_UNKNOWN)
    app->cfg.pres_ev = EVTYPE_PRES;

  if (app->cfg.pres_ev == EVTYPE_UNKNOWN) {
    hvalue = pj_str("keep-alive");
  } else if (app->cfg.pres_ev == EVTYPE_PRES) {
    hvalue = pj_str("presence");
  } else if (app->cfg.pres_ev == EVTYPE_MWI) {
    hvalue = pj_str("message-summary");
  } else {
    hvalue = app->cfg.event; // EVTYPE_OTHER
  }

  event_hdr = pjsip_generic_string_hdr_create(app->pool, &hname, &hvalue);
  pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*) event_hdr);
}

static void add_content_type_hdr(pjsip_tx_data *tdata, struct sippak_app *app)
{
  pj_str_t hname = pj_str("Content-Type");
  pj_str_t hvalue;
  pjsip_generic_string_hdr *hdr;
  char buf[512];
  int len = 0;

  if (app->cfg.ctype_e == CTYPE_UNKNOWN) {
    return;
  }
  len = pjsip_media_type_print(buf, 512, &app->cfg.ctype_media);
  if (len == -1) {
    PJ_LOG(1, (NAME, "Failed to allocate memory for content-type header value."));
    return;
  }
  hvalue.slen = len;
  hvalue.ptr = buf;

  hdr = pjsip_generic_string_hdr_create(app->pool, &hname, &hvalue);
  pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*) hdr);
}

static void add_body_msg (pjsip_tx_data *tdata, struct sippak_app *app)
{
  pj_status_t status;
  char buf[SIPMSG_BODY_LEN];
  pj_str_t body_txt;
  pjsip_msg_body *body;
  pjsip_pres_status pres_status;

  // --- pres status for pidf/xpidf
  // TODO: move to sip_helper.c and use it in publish.c
  pj_bzero(&pres_status, sizeof(pres_status));
  pres_status.info_cnt = 1;
  pres_status.info[0].basic_open = app->cfg.pres_status_open;
  pres_status.info[0].id = app->cfg.dest;
  pres_status.info[0].rpid.note = app->cfg.pres_note;
  pres_status.info[0].rpid.activity = app->cfg.pres_status_open
    ? PJRPID_ACTIVITY_UNKNOWN
    : PJRPID_ACTIVITY_BUSY;

  if (app->cfg.is_mwi == PJ_TRUE) {
    body_txt.slen = pj_ansi_snprintf (buf, SIPMSG_BODY_LEN,
        "Messages-Waiting: %s\r\n"
        "Message-Account: %.*s\r\n"
        "Voice-Message: %d/%d (%d/%d)\r\n",
        app->cfg.mwi[0] > 0 ? "yes" : "no",
        (int)app->cfg.mwi_acc.slen, app->cfg.mwi_acc.ptr,
        app->cfg.mwi[0], app->cfg.mwi[1], app->cfg.mwi[2], app->cfg.mwi[3]
        );
    body_txt.ptr = buf;
    tdata->msg->body = pjsip_msg_body_create(app->pool,
        &app->cfg.ctype_media.type,
        &app->cfg.ctype_media.subtype,
        &body_txt);
  } else if (app->cfg.ctype_e == CTYPE_PIDF) {
    status = pjsip_pres_create_pidf(tdata->pool, &pres_status, &app->cfg.dest, &tdata->msg->body);
    if (status != PJ_SUCCESS)
      PJ_LOG(2, (NAME, "Failed to create PIDF body."));
  } else if (app->cfg.ctype_e == CTYPE_XPIDF) {
    status = pjsip_pres_create_xpidf(tdata->pool, &pres_status, &app->cfg.dest, &tdata->msg->body);
    if (status != PJ_SUCCESS)
      PJ_LOG(2, (NAME, "Failed to create XPIDF body."));
  } else {
    add_content_type_hdr(tdata, app);
  }
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
PJ_DEF(pj_status_t) sippak_cmd_notify (struct sippak_app *app)
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
              pjsip_get_notify_method(), // method NOTIFY
              &ruri,                  // request URI
              &from,                  // from header value
              &app->cfg.dest,         // to header value
              &cnt,                   // Contact header
              NULL,                   // Call-ID
              -1,                     // CSeq
              NULL,                   // body
              &tdata);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  add_event_hdr(tdata, app);

  add_body_msg(tdata, app);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_register_module(app->endpt, &mod_notify);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  return pjsip_endpt_send_request(app->endpt, tdata, -1, app, &send_cb);
}
