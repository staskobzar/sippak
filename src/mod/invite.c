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
#include <pjsip_ua.h>
#include "sippak.h"

#define NAME "mod_invite"

static pj_bool_t on_rx_response (pjsip_rx_data *rdata);

static pjsip_module mod_invite =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-invite", 10 },       /* Name.    */
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

  status = sippak_transport_init(app, &local_addr, &local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_tsx_layer_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_ua_init_module(app->endpt, NULL);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* Initialize 100rel support */
  status = pjsip_100rel_init_module(app->endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = pjsip_endpt_register_module(app->endpt, &mod_invite);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* BEGIN: Media */
  /* END: Media */

  cnt  = sippak_create_contact_hdr(app, local_addr, local_port);
  from = sippak_create_from_hdr(app);
  ruri = sippak_create_ruri(app);

  status = pjsip_dlg_create_uac(pjsip_ua_instance(),
      &from, &cnt, &ruri, &ruri, &dlg);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* invite session */
  status = pjsip_inv_create_uac( dlg, NULL, 0, &inv);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* create invite request */
  status = pjsip_inv_invite(inv, &tdata);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  /* send invite */
  return pjsip_inv_send_msg(inv, tdata);
}
