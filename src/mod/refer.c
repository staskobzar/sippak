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
 * @brief sippak send REFER request. Click To Dial scenrio (rfc#5359 section 2.18)
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjsip_ua.h>
#include "sippak.h"

#define NAME "mod_refer"

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);
static void send_cb(void *token, pjsip_event *e);
static void add_referto_hdr(pjsip_tx_data *tdata, struct sippak_app *app);
static void add_refersub_hdr(pjsip_tx_data *tdata, struct sippak_app *app);

static pjsip_module mod_refer =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-refer", 9 },     /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_TSX_LAYER - 1, /* Priority          */
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

static void add_referto_hdr(pjsip_tx_data *tdata, struct sippak_app *app)
{
  pj_str_t hname = pj_str("Refer-To");
  pjsip_generic_string_hdr *hdr;
  hdr = pjsip_generic_string_hdr_create(app->pool, &hname, &app->cfg.refer_to);
  pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*) hdr);
}

static void add_refersub_hdr(pjsip_tx_data *tdata, struct sippak_app *app)
{
  pj_str_t hname = pj_str("Refer-Sub");
  pj_str_t hvalue = pj_str("false");
  pjsip_generic_string_hdr *hdr = pjsip_generic_string_hdr_create(app->pool, &hname, &hvalue);
  pjsip_msg_add_hdr(tdata->msg, (pjsip_hdr*) hdr);
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
  if (via && via->rport_param) {
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
  puts("==============> send_cb");
}

/* Refer */
pj_status_t sippak_cmd_refer (struct sippak_app *app)
{
  pj_status_t status;
  pj_str_t *local_addr;
  int local_port;
  pjsip_tx_data *tdata = NULL;
  pj_str_t ruri, from_uri, cnt;

  status = sippak_transport_init(app, &local_addr, &local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_register_module(app->endpt, &mod_refer);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  ruri     = sippak_create_ruri(app);
  from_uri = sippak_create_from_hdr(app);
  cnt      = sippak_create_contact_hdr(app, local_addr, local_port);

  status = pjsip_endpt_create_request(app->endpt,
              pjsip_get_refer_method(), // method REFER
              &ruri,                  // request URI
              &from_uri,              // from header value
              &app->cfg.dest,         // to header value
              &cnt,                   // Contact header
              NULL,                   // Call-ID
              -1,                     // CSeq
              NULL,                   // body
              &tdata);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  add_referto_hdr(tdata, app);
  add_refersub_hdr(tdata, app);

  return pjsip_endpt_send_request(app->endpt, tdata, -1, app, &send_cb);
}
