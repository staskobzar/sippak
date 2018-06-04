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
 * @brief sippak INVITE session
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjmedia.h>
#include <pjmedia-codec.h>
#include <pjsip_ua.h>
#include "sippak.h"

#define NAME "mod_invite"

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);
static void call_on_state_changed( pjsip_inv_session *inv, pjsip_event *e);
static void call_on_forked(pjsip_inv_session *inv, pjsip_event *e);

static pjsip_module mod_invite =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-sippak-inv", 14 },           /* Name. Name mod-invite is reserved by pjsip   */
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

/* On response module callback */
static pj_bool_t on_rx_response (pjsip_rx_data *rdata)
{
  return PJ_FALSE; // continue with othe modules
}

static void call_on_state_changed( pjsip_inv_session *inv, pjsip_event *e)
{
  PJ_UNUSED_ARG(e);
  pj_status_t status;
  pjsip_tx_data *tdata;

  printf("=======> call_on_state_changed. STATE: %s\n", pjsip_inv_state_name(inv->state));

  if (inv->state == PJSIP_INV_STATE_DISCONNECTED) {
    PJ_LOG(3, (NAME, "Call completed."));
    sippak_loop_cancel();
  } else if (inv->state == PJSIP_INV_STATE_CONFIRMED) {
    PJ_LOG(3, (NAME, "Call confirmed. Now terminating with BYE."));
    status = pjsip_inv_end_session(inv, PJSIP_SC_OK, NULL, &tdata);
    if (status==PJ_SUCCESS && tdata)
      pjsip_inv_send_msg(inv, tdata);
  }
}

static void call_on_forked(pjsip_inv_session *inv, pjsip_event *e)
{
  PJ_UNUSED_ARG(e);
  PJ_UNUSED_ARG(inv);
}

/* Ping */
pj_status_t sippak_cmd_invite (struct sippak_app *app)
{
  pj_status_t status;
  pjsip_dialog *dlg = NULL;
  pjsip_tx_data *tdata;
  pj_str_t *local_addr;
  int local_port;
  pj_str_t cnt, from, ruri;
  pjsip_inv_session *inv;
  pjsip_inv_callback inv_cb;

  status = sippak_transport_init(app, &local_addr, &local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_ua_init_module(app->endpt, NULL);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* Initialize 100rel support */
  status = pjsip_100rel_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* invite usage module */
  pj_bzero(&inv_cb, sizeof(inv_cb));
  inv_cb.on_state_changed = &call_on_state_changed;
  inv_cb.on_new_session = &call_on_forked;
  status = pjsip_inv_usage_init(app->endpt, &inv_cb);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_register_module(app->endpt, &mod_invite);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* BEGIN: Media */
  pjmedia_sock_info sock_info;
  pjmedia_transport *med_transport;
  pjmedia_endpt *med_endpt;
  pjmedia_transport_info med_tpinfo;
  pjmedia_sdp_session *sdp_sess;

  status = pjmedia_endpt_create(&app->cp->factory, NULL, 1, &med_endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
#if defined(PJMEDIA_HAS_G711_CODEC) && PJMEDIA_HAS_G711_CODEC!=0
  puts("======> init codec g711");
  pjmedia_codec_g711_init(med_endpt);
#elif defined(DPJMEDIA_HAS_SPEEX_CODEC) && DPJMEDIA_HAS_SPEEX_CODEC!=0
  puts("======> init codec speex");
  pjmedia_codec_speex_init_default(med_endpt);
#elif defined(DPJMEDIA_HAS_ILBC_CODEC) && DPJMEDIA_HAS_ILBC_CODEC!=0
  puts("======> init codec ilbc");
  pjmedia_codec_ilbc_init(med_endpt);
#elif defined(DPJMEDIA_HAS_GSM_CODEC) && DPJMEDIA_HAS_GSM_CODEC!=0
  puts("======> init codec gsm");
  pjmedia_codec_gsm_init(med_endpt);
#elif defined(DPJMEDIA_HAS_G722_CODEC) && DPJMEDIA_HAS_G722_CODEC!=0
  puts("======> init codec g722");
  pjmedia_codec_g722_init(med_endpt);
#endif

  status = pjmedia_transport_udp_create(med_endpt,
      NAME,     // transport name
      10000,    // rtp port
      0,        // options
      &med_transport);
  pjmedia_transport_info_init(&med_tpinfo);
  pjmedia_transport_get_info(med_transport, &med_tpinfo);

  pj_memcpy(&sock_info, &med_tpinfo.sock_info, sizeof(pjmedia_sock_info));

  status = pjmedia_endpt_create_sdp( med_endpt,
      app->pool,
      1, // # of streams
      &sock_info,  // rtp sock
      &sdp_sess); // local sdp
  /* END: Media */

  cnt  = sippak_create_contact_hdr(app, local_addr, local_port);
  from = sippak_create_from_hdr(app);
  ruri = sippak_create_ruri(app);

  status = pjsip_dlg_create_uac(pjsip_ua_instance(),
      &from, &cnt, &ruri, &ruri, &dlg);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* invite session */
  status = pjsip_inv_create_uac( dlg, sdp_sess, 0, &inv);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* create invite request */
  status = pjsip_inv_invite(inv, &tdata);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* send invite */
  return pjsip_inv_send_msg(inv, tdata);
}
