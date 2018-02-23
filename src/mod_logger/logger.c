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
 * @file logger.c
 * @brief sippak logger module
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <pjsip.h>
#include <pjlib.h>
#include "logger.h"

static pjsip_module msg_logger[] =
{{
    NULL, NULL,                 /* prev, next.    */
    { "mod-logger", 11 },      /* Name.    */
    -1,                         /* Id      */
    PJSIP_MOD_PRIORITY_TRANSPORT_LAYER - 1, /* Priority          */
    NULL,                       /* load()    */
    NULL,                       /* start()    */
    NULL,                       /* stop()    */
    NULL,                       /* unload()    */
    &logging_on_rx_msg,         /* on_rx_request()  */
    &logging_on_rx_msg,         /* on_rx_response()  */
    &logging_on_tx_msg,         /* on_tx_request.  */
    &logging_on_tx_msg,         /* on_tx_response()  */
    NULL,                       /* on_tsx_state()  */

}};

/* Notification on incoming messages */
static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata)
{
  PJ_LOG(1,("FOO", "RX %d bytes %s from %s %s:%d:\n"
        "%.*s\n"
        "--end msg--",
        rdata->msg_info.len,
        pjsip_rx_data_get_info(rdata),
        rdata->tp_info.transport->type_name,
        rdata->pkt_info.src_name,
        rdata->pkt_info.src_port,
        (int)rdata->msg_info.len,
        rdata->msg_info.msg_buf));
  return PJ_FALSE;
}

/* Notification on outgoing messages */
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata)
{
  PJ_LOG(1,("FOO", "TX %d bytes %s to %s %s:%d:\n"
        "%.*s\n"
        "--end msg--",
        (tdata->buf.cur - tdata->buf.start),
        pjsip_tx_data_get_info(tdata),
        tdata->tp_info.transport->type_name,
        tdata->tp_info.dst_name,
        tdata->tp_info.dst_port,
        (int)(tdata->buf.cur - tdata->buf.start),
        tdata->buf.start));
  return PJ_SUCCESS;
}

pj_status_t sippak_mod_logger_register(pjsip_endpoint *endpt)
{
  return pjsip_endpt_register_module(endpt, msg_logger);
}
