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
#include <pjsip/print_util.h>
#include "sippak.h"
#include "logger.h"

#define NAME "mod_logger"

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

static void term_set_color(int level)
{
  if (ENABLE_COLORS != PJ_TRUE)
    return;
#if defined(PJ_TERM_HAS_COLOR) && PJ_TERM_HAS_COLOR != 0
    pj_term_set_color(level);
#else
    PJ_UNUSED_ARG(level);
#endif
}

static void term_restore_color(void)
{
  if (ENABLE_COLORS != PJ_TRUE)
    return;
#if defined(PJ_TERM_HAS_COLOR) && PJ_TERM_HAS_COLOR != 0
    /* Set terminal to its default color */
    pj_term_set_color(pj_log_get_color(77));
#endif
}

static void print_trail_chr ()
{
  if (PRINT_TRAIL_CHR == PJ_TRUE) {
    term_set_color (PJ_TERM_COLOR_BRIGHT |
                    PJ_TERM_COLOR_R |
                    PJ_TERM_COLOR_G |
                    PJ_TERM_COLOR_B);
    printf("%c", TRAIL_CHR);
  }
  printf("\n");
}

static void print_sipmsg_body(pjsip_msg_body *body)
{
  char clenh_holder[256] = {0};
  char len[12] = {0};
  pj_str_t clen = { clenh_holder, 0 };

  pj_strcat2(&clen, pjsip_hdr_names[PJSIP_H_CONTENT_LENGTH].name);
  pj_strcat2(&clen, ": ");

  if (body == NULL) {
    pj_strcat2(&clen, "0");
  } else {
    pj_utoa(body->len, len);
    pj_strcat2(&clen, len);
  }

  print_generic_header(clen.ptr, clen.slen);

  if (body) {
    term_set_color (PJ_TERM_COLOR_B);
    printf("\n[SIP MESSAGE BODY]\n");
    term_restore_color ();
  }
}

static void print_sipmsg_head(pjsip_msg *msg)
{
  char uri_buf[128] = { 0 };
  int uri_len = 0;

  if (msg->type == PJSIP_RESPONSE_MSG) {
    term_set_color (PJ_TERM_COLOR_B);
    printf("SIP");
    term_set_color (PJ_TERM_COLOR_R);
    printf("/");
    term_set_color (PJ_TERM_COLOR_G | PJ_TERM_COLOR_B);
    printf("2.0 %d ", msg->line.status.code);
    term_set_color (PJ_TERM_COLOR_G);
    printf("%.*s", (int)msg->line.status.reason.slen, msg->line.status.reason.ptr);

  } else {

    term_set_color (PJ_TERM_COLOR_G);
    printf("%.*s ", (int)msg->line.req.method.name.slen, msg->line.req.method.name.ptr);

    uri_len = pjsip_uri_print (PJSIP_URI_IN_REQ_URI,
        msg->line.req.uri, uri_buf, 128);
    term_set_color (PJ_TERM_COLOR_G | PJ_TERM_COLOR_B);
    printf("%.*s ", uri_len, uri_buf);
    term_set_color (PJ_TERM_COLOR_G);
    printf("SIP/2.0");
  }

  print_trail_chr();

  term_restore_color ();
}

static void print_generic_header (const char *header, int len)
{
  char *hname_col = strchr(header, ':');
  if (hname_col == NULL) {
    PJ_LOG(1, (NAME, "Invalid SIP header: %s", header));
    return;
  }
  int hname_len = hname_col - header;
  int hval_len = len - (hname_col - header) - 1;

  term_set_color (PJ_TERM_COLOR_G);
  printf("%.*s", hname_len, header); // SIP header name
  term_set_color (PJ_TERM_COLOR_R);
  printf(":");
  term_set_color (PJ_TERM_COLOR_R | PJ_TERM_COLOR_G);
  printf("%.*s", hval_len, hname_col + 1);
  print_trail_chr();
  term_restore_color ();
}

/* Notification on incoming messages */
static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata)
{
  pjsip_msg *msg = rdata->msg_info.msg;
  pjsip_hdr *hdr = NULL;
  char value[ 512 ] = { 0 };

  PJ_LOG(3, (PROJECT_NAME, "RX %d bytes %s from %s %s:%d:\n",
        rdata->msg_info.len,
        pjsip_rx_data_get_info(rdata),
        rdata->tp_info.transport->type_name,
        rdata->pkt_info.src_name,
        rdata->pkt_info.src_port,
        (int)rdata->msg_info.len));

  print_sipmsg_head(msg);

  for (hdr=msg->hdr.next; hdr!=&msg->hdr; hdr=hdr->next) {
    int len = hdr->vptr->print_on( hdr, value, 512 );
    print_generic_header (value, len);
  }
  puts("");
  return PJ_FALSE; // continue with othe modules
}

/* Notification on outgoing messages */
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata)
{
  pjsip_msg *msg = tdata->msg;
  pjsip_hdr *hdr = NULL;
  char value[ 512 ] = { 0 };

  PJ_LOG(3, (PROJECT_NAME, "TX %d bytes %s to %s %s:%d:\n",
        (tdata->buf.cur - tdata->buf.start),
        pjsip_tx_data_get_info(tdata),
        tdata->tp_info.transport->type_name,
        tdata->tp_info.dst_name,
        tdata->tp_info.dst_port));

  print_sipmsg_head(msg);

  for (hdr=msg->hdr.next; hdr!=&msg->hdr; hdr=hdr->next) {
    int len = hdr->vptr->print_on( hdr, value, 512 );
    print_generic_header (value, len);
  }

  print_sipmsg_body (msg->body);

  puts("");
  return PJ_SUCCESS; //continue with other modules
}

pj_status_t sippak_mod_logger_register(struct sippak_app *app)
{
  ENABLE_COLORS = (app->cfg.log_decor & PJ_LOG_HAS_COLOR)
    ? PJ_TRUE
    : PJ_FALSE;

  PRINT_TRAIL_CHR = app->cfg.trail_dot;

  return pjsip_endpt_register_module(app->endpt, msg_logger);
}
