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
 * @file auth.c
 * @brief sippak authentication module
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include "sippak.h"

#define NAME "mod_auth"

static pjsip_auth_clt_sess auth_sess;

static pj_bool_t on_rx_response(pjsip_rx_data *rdata);

static pjsip_module mod_auth =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-auth", 9 },          /* Name.    */
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
  NULL,                       /* on_tsx_state()  */
};

static pj_bool_t on_rx_response (pjsip_rx_data *rdata)
{
  int code = 0;
  pj_status_t status;

  if (rdata->msg_info.msg->type != PJSIP_RESPONSE_MSG) {
    return PJ_FALSE;
  }

  code = rdata->msg_info.msg->line.status.code;

  if (code == PJSIP_SC_PROXY_AUTHENTICATION_REQUIRED) {
    PJ_LOG(3, (NAME, "Proxy Authentication required. The request requires user authentication."));
  } else if (code == PJSIP_SC_UNAUTHORIZED) {
    PJ_LOG(3, (NAME, "Unauthorized. The request requires user authentication."));
  } else {
    return PJ_FALSE; // not authentication challenge
  }

  PJ_LOG(3, (NAME, "===> Setting cred info"));
  pjsip_cred_info	cred[1];
  cred->realm     = pj_str("*");
  cred->scheme    = pj_str("digest");
  cred->username  = pj_str("alice");
  cred->data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
  cred->data      = pj_str("pa55w0rd");

  PJ_LOG(3, (NAME, "===> pjsip_auth_clt_set_credentials"));
  pjsip_auth_clt_set_credentials(&auth_sess, 1, cred);

  pjsip_transaction *tsx = pjsip_rdata_get_tsx(rdata);
  pjsip_tx_data *tdata;

  PJ_LOG(3, (NAME, "===> pjsip_auth_clt_reinit_req user: %s", auth_sess.cred_info->username));
  status = pjsip_auth_clt_reinit_req(&auth_sess, rdata,
      tsx->last_tx, &tdata);
  if (status != PJ_SUCCESS) {
    PJ_LOG(1, (NAME, "Failed to re-init client authentication session."));
    return PJ_FALSE;
  }

  PJ_LOG(3, (NAME, "===> pjsip_tsx_send_msg"));
  status = pjsip_tsx_send_msg(tsx, tdata);
  if (status != PJ_SUCCESS) {
    PJ_LOG(1, (NAME, "Failed to send message with authentication."));
    return PJ_FALSE;
  }

  return PJ_TRUE; // continue with othe modules
}

pj_status_t sippak_mod_auth_register(struct sippak_app *app)
{
  pj_status_t status;

  status = pjsip_auth_clt_init (&auth_sess, app->endpt, app->pool, 0);

  if (status != PJ_SUCCESS) {
    PJ_LOG(1, (NAME, "Failed to init client authentication session."));
    return status;
  }

  return pjsip_endpt_register_module(app->endpt, &mod_auth);
}
