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
  NULL,                       /* on_rx_response()  */
  NULL,                       /* on_tx_request.  */
  NULL,                       /* on_tx_response()  */
  NULL,                       /* on_tsx_state()  */
};

//
void on_evsub_state(pjsip_evsub *sub, pjsip_event *event)
{}
void on_tsx_state(pjsip_evsub *sub, pjsip_transaction *tsx, pjsip_event *event)
{}
void on_rx_refresh(pjsip_evsub *sub, pjsip_rx_data *rdata, int *p_st_code, pj_str_t **p_st_text, pjsip_hdr *res_hdr, pjsip_msg_body **p_body)
{}
void on_rx_notify(pjsip_evsub *sub, pjsip_rx_data *rdata, int *p_st_code, pj_str_t **p_st_text, pjsip_hdr *res_hdr, pjsip_msg_body **p_body)
{}
void on_client_refresh(pjsip_evsub *sub)
{}
void on_server_timeout(pjsip_evsub *sub)
{}
pjsip_evsub_user pres_user = {
  &on_evsub_state,
  &on_tsx_state,
  &on_rx_refresh,
  &on_rx_notify,
  &on_client_refresh,
  &on_server_timeout
};
//

pj_status_t sippak_cmd_subscribe (struct sippak_app *app)
{
  pj_status_t status;
  pjsip_dialog *dlg = NULL;
  pjsip_tx_data *tdata;
  pjsip_evsub *sub;
  pj_str_t cnt, from, ruri;
  pj_str_t *local_addr;
  int local_port;


  status = sippak_transport_init(app, &local_addr, &local_port);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  cnt  = sippak_create_contact_hdr(app, local_addr, local_port);
  from = sippak_create_from_hdr(app);
  ruri = sippak_create_ruri(app);

  status = pjsip_ua_init_module(app->endpt, NULL);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  puts("===> pjsip_dlg_create_uac");
  status = pjsip_dlg_create_uac(pjsip_ua_instance(),
      &from, &cnt, &ruri, &ruri, &dlg);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  puts("===> pjsip_evsub_init_module");
  /* Init core SIMPLE module : */
  pjsip_evsub_init_module(app->endpt);
  puts("===> pjsip_pres_init_module");
  pjsip_pres_init_module(app->endpt, &mod_subscribe);
  puts("===> pjsip_endpt_register_module");
  pjsip_endpt_register_module(app->endpt, &mod_subscribe);
  puts("===> pjsip_pres_create_uac");
  pjsip_pres_create_uac(dlg, &pres_user, 1, &sub);
  puts("===> pjsip_pres_initiate");
  pjsip_pres_initiate(sub, -1, &tdata); // expire is here

  puts("===> pjsip_tsx_layer_init_module");
  pjsip_tsx_layer_init_module(app->endpt);
  puts("===> pjsip_pres_send_request");
  status = pjsip_pres_send_request(sub, tdata);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  return status;
}
