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

pj_status_t sippak_cmd_subscribe (struct sippak_app *app)
{
  pj_status_t status;
  status = pjsip_endpt_register_module(app->endpt, &mod_subscribe);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  // return pjsip_publishc_send(publish_sess, tdata);
  return status;
}
