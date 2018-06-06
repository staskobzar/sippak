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
 * @brief sippak mangling SIP packets before send
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include "sippak.h"

#define NAME "mod_sip_mangler"

static struct sippak_app *local_app;
static pj_status_t on_tx_request(pjsip_tx_data *tdata);

static pjsip_module mod_sip_mangler =
{
  NULL, NULL,                 /* prev, next.    */
  { "mod-sip-mangler", 15 },          /* Name.    */
  -1,                         /* Id      */
  PJSIP_MOD_PRIORITY_TSX_LAYER, /* Priority          */
  NULL,                       /* load()    */
  NULL,                       /* start()    */
  NULL,                       /* stop()    */
  NULL,                       /* unload()    */
  NULL,                       /* on_rx_request()  */
  NULL,                       /* on_rx_response()  */
  &on_tx_request,             /* on_tx_request.  */
  NULL,                       /* on_tx_response()  */
  NULL,                       /* on_tsx_state()  */
};

static pj_status_t on_tx_request(pjsip_tx_data *tdata)
{
  sippak_add_sip_headers (tdata, local_app);
}

/* Ping */
pj_status_t sippak_mod_sip_mangler_register (struct sippak_app *app)
{
  local_app = app;
  return pjsip_endpt_register_module(app->endpt, &mod_sip_mangler);
}
