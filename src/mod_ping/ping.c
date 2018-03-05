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

static pj_status_t on_tx_response (pjsip_tx_data *rdata);
static pj_bool_t on_rx_response (pjsip_rx_data *rdata);

static pjsip_module mod_ping =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-ping", 9 },      /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_APPLICATION, /* Priority          */
  NULL,                       /* load()    */
  NULL,                       /* start()    */
  NULL,                       /* stop()    */
  NULL,                       /* unload()    */
  NULL,         /* on_rx_request()  */
  &on_rx_response,         /* on_rx_response()  */
  NULL,         /* on_tx_request.  */
  NULL,         /* on_tx_response()  */
  NULL,                       /* on_tsx_state()  */
};

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

pj_status_t sippak_cmd_ping (struct sippak_app *app)
{
  pj_status_t status;
  pj_str_t str;
  pj_sockaddr_in addr;
  pjsip_transport *tp;
  pjsip_tx_data *tdata;

  status = pjsip_endpt_register_module(app->endpt, &mod_ping);

  pj_sockaddr_in_init(&addr, NULL, 0);

  status = pjsip_endpt_create_request(app->endpt,
              &pjsip_options_method,  // method OPTIONS
              pj_cstr(&str, app->cfg.dest), // request URI
              pj_cstr(&str, app->cfg.dest), // from header value
              pj_cstr(&str, app->cfg.dest), // to header value
              pj_cstr(&str, app->cfg.dest), // Contact header
              NULL,                   // Call-ID
              -1,                     // CSeq
              NULL,                   // body
              &tdata);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_udp_transport_start( app->endpt, &addr, NULL, 1, &tp);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_acquire_transport(app->endpt, PJSIP_TRANSPORT_UDP, &addr, sizeof(addr), NULL, &tp);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  return pjsip_endpt_send_request_stateless(app->endpt, tdata, NULL, NULL);
}
