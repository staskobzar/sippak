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
 * @file publish.c
 * @brief sippak SUBSCRIBE message send
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjsip_simple.h>
#include <pjlib-util.h>

#include <pjsip-simple/evsub.h>
#include <pjsip-simple/presence.h>
#include "sippak.h"

#define NAME "mod_subscribe"

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);
static void on_evsub_state(pjsip_evsub *sub, pjsip_event *event);
static void on_tsx_state(pjsip_evsub *sub, pjsip_transaction *tsx, pjsip_event *event);
static void on_rx_notify(pjsip_evsub *sub, pjsip_rx_data *rdata, int *p_st_code,
    pj_str_t **p_st_text, pjsip_hdr *res_hdr, pjsip_msg_body **p_body);
static void on_tsx_state(pjsip_evsub *sub, pjsip_transaction *tsx, pjsip_event *event);
static sippak_evtype_e set_sub_evtype(struct sippak_app *app);
static void set_dlg_outbound_proxy(pjsip_dialog *dlg, struct sippak_app *app);
static short unsigned auth_tries = 0;

static pjsip_module mod_subscribe =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-publish", 11 },      /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_TSX_LAYER - 1, /* Priority */
  NULL,                       /* load()    */
  NULL,                       /* start()    */
  NULL,                       /* stop()    */
  NULL,                       /* unload()    */
  NULL,                       /* on_rx_request()  */
  &on_rx_response,                       /* on_rx_response()  */
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

  if (msg->line.status.code == PJSIP_SC_PROXY_AUTHENTICATION_REQUIRED ||
      msg->line.status.code == PJSIP_SC_UNAUTHORIZED) {
    auth_tries++;
    if (auth_tries > 1) {
      sippak_loop_cancel();
      PJ_LOG(1, (NAME, "Authentication failed. Check your username and password"));
      return PJ_TRUE;
    }
    return PJ_FALSE; // processed with callback
  }

  // sippak_loop_cancel();

  return PJ_FALSE; // continue with othe modules
}

//
//
static void on_evsub_state(pjsip_evsub *sub, pjsip_event *event)
{
  PJ_UNUSED_ARG(event);
  pjsip_evsub_state state = pjsip_evsub_get_state(sub);
  // printf("-------> on_evsub_state %d : %s\n", pjsip_evsub_get_state(sub), pjsip_evsub_get_state_name(sub));
  // Stop when subscription is activated
  if (state == PJSIP_EVSUB_STATE_ACTIVE) {
    PJ_LOG(3, (NAME,
          "[v] Subscription is activated."));
    sippak_loop_cancel();
  } else if (state == PJSIP_EVSUB_STATE_TERMINATED) {
    PJ_LOG(3, (NAME,
          "[x] Subscription is terminated."));
    sippak_loop_cancel();
  }
}

static void on_rx_notify(pjsip_evsub *sub,
                  pjsip_rx_data *rdata,
                  int *p_st_code,
                  pj_str_t **p_st_text,
                  pjsip_hdr *res_hdr,
                  pjsip_msg_body **p_body)
{
  PJ_UNUSED_ARG(sub);
  PJ_UNUSED_ARG(rdata);
  PJ_UNUSED_ARG(p_st_code);
  PJ_UNUSED_ARG(*p_st_text);
  PJ_UNUSED_ARG(res_hdr);
  PJ_UNUSED_ARG(*p_body);
  // printf("-------> on_rx_notify p_st_text %.*s\n", (*p_st_text)->slen, (*p_st_text)->ptr);
  // printf("-------> on_rx_notify p_st_code %d\n", *p_st_code);
}

static void on_tsx_state(pjsip_evsub *sub, pjsip_transaction *tsx, pjsip_event *event)
{
  PJ_UNUSED_ARG(sub);
  PJ_UNUSED_ARG(event);
  if (tsx->status_code == PJSIP_SC_TSX_TIMEOUT) {
    PJ_LOG(1,
        (NAME, "Response timeout after %d retransmitions.", tsx->retransmit_count));
    sippak_loop_cancel();
  }
}

static sippak_evtype_e set_sub_evtype(struct sippak_app *app)
{
  if (app->cfg.pres_ev == EVTYPE_OTHER || app->cfg.pres_ev == EVTYPE_UNKNOWN) {
    PJ_LOG(2,
        (NAME, "Presence event \"%.*s\" is not supported. Will use \"presence\".",
         app->cfg.event.slen, app->cfg.event.ptr));
    return EVTYPE_PRES;
  }
  return app->cfg.pres_ev;
}

static void set_dlg_outbound_proxy(pjsip_dialog *dlg, struct sippak_app *app)
{
  pjsip_route_hdr route_set;
  int i;
  const pj_str_t hname = { "Route", 5 };

  if (app->cfg.proxy.cnt == 0) {
    return;
  }

  pj_list_init(&route_set);

  for(i = 0; i < app->cfg.proxy.cnt; i++) {
    pjsip_route_hdr *route = pjsip_parse_hdr(dlg->pool, &hname,
        app->cfg.proxy.p[i], pj_ansi_strlen(app->cfg.proxy.p[i]),
        NULL);
    if (route) {
      pj_list_push_back(&route_set, route);
    }
  }
  pjsip_dlg_set_route_set(dlg, &route_set);
}

pj_status_t sippak_cmd_subscribe (struct sippak_app *app)
{
  pj_status_t status;
  pjsip_dialog *dlg = NULL;
  pjsip_tx_data *tdata;
  pjsip_evsub *sub;
  pj_str_t cnt, from, ruri;
  pj_str_t *local_addr;
  int local_port;
  pjsip_evsub_user pres_cb;
  pjsip_cred_info cred[1];

  sippak_evtype_e evtype = set_sub_evtype(app);

  status = sippak_transport_init(app, &local_addr, &local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  cnt  = sippak_create_contact_hdr(app, local_addr, local_port);
  from = sippak_create_from_hdr(app);
  ruri = sippak_create_ruri(app);

  status = pjsip_ua_init_module(app->endpt, NULL);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_dlg_create_uac(pjsip_ua_instance(),
      &from, &cnt, &ruri, &ruri, &dlg);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  // TODO: set routes for subscribe
  // set_dlg_outbound_proxy(dlg, app);

  sippak_set_cred(app, cred);
  status = pjsip_auth_clt_set_credentials(&dlg->auth_sess, 1, cred);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* Init core SIMPLE module : */
  status = pjsip_evsub_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  if (app->cfg.pres_ev == EVTYPE_MWI) {
    status = pjsip_mwi_init_module(app->endpt, &mod_subscribe);
  } else {
    status = pjsip_pres_init_module(app->endpt, &mod_subscribe);
  }
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_register_module(app->endpt, &mod_subscribe);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pj_bzero(&pres_cb, sizeof(pres_cb));
  pres_cb.on_tsx_state = &on_tsx_state;
  pres_cb.on_evsub_state = &on_evsub_state;
  pres_cb.on_rx_notify = &on_rx_notify;

  if (app->cfg.pres_ev == EVTYPE_MWI) {
    status = pjsip_mwi_create_uac(dlg, &pres_cb, 1, &sub);
  } else {
    status = pjsip_pres_create_uac(dlg, &pres_cb, 1, &sub);
  }
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  if (app->cfg.pres_ev == EVTYPE_MWI) {
    status = pjsip_mwi_initiate(sub, app->cfg.expires, &tdata);
  } else {
    status = pjsip_pres_initiate(sub, app->cfg.expires, &tdata);
  }
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  if (app->cfg.pres_ev == EVTYPE_MWI) {
    status = pjsip_mwi_send_request(sub, tdata);
  } else {
    status = pjsip_pres_send_request(sub, tdata);
  }
  return status;
}
