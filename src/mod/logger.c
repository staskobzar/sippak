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

static pjsip_module msg_logger = {
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

};

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
    PRINT_COLOR(COLOR_BRIGHT_WHITE, "%c", TRAIL_CHR);
  }
  printf("\n");
}

static void print_content_len_hdr (unsigned int len)
{
  char hdr_holder[512] = {0};
  char str_len[12] = {0};
  pj_str_t str_hdr = { hdr_holder, 0 };

  pj_strcat2(&str_hdr, pjsip_hdr_names[PJSIP_H_CONTENT_LENGTH].name);
  pj_strcat2(&str_hdr, ": ");

  pj_utoa(len, str_len);
  pj_strcat2(&str_hdr, str_len);

  print_generic_header(str_hdr.ptr, str_hdr.slen);
}

static void print_content_type_hdr (const pjsip_msg_body *body)
{
  char hdr_holder[512] = {0};
  pj_str_t str_hdr = { hdr_holder, 0 };

  if (!body->content_type.type.slen || !body->content_type.subtype.slen) {
    return;
  }
  pj_strcat2(&str_hdr, pjsip_hdr_names[PJSIP_H_CONTENT_TYPE].name);
  pj_strcat2(&str_hdr, ": ");
  pj_strcat(&str_hdr, &body->content_type.type);
  pj_strcat2(&str_hdr, "/");
  pj_strcat(&str_hdr, &body->content_type.subtype);

  print_generic_header(str_hdr.ptr, str_hdr.slen);
}

static void print_sipmsg_body(pjsip_msg *msg, pj_bool_t is_tx)
{
  char buf[SIPMSG_BODY_LEN];
  short len = 0;

  if (msg->body) {
    len = msg->body->print_body(msg->body, buf, SIPMSG_BODY_LEN);
    if (len == -1) {
      PJ_LOG(1, (NAME, "ERROR in logger.c: Message body buffer is too short (%d).",
            SIPMSG_BODY_LEN));
      return;
    }
    // print content-type and length headers only for incoming messages
    // outgoing messages have those headers and print as generic headers
    if (is_tx) {
      print_content_type_hdr (msg->body);
      print_content_len_hdr (len);
    }
    PRINT_COLOR(COLOR_CYAN, "\n%.*s\n", len, buf);
    term_restore_color();
  } else {
    if (is_tx)
      print_content_len_hdr (len);
  }
}

static void print_sipmsg_head(pjsip_msg *msg)
{
  char uri_buf[128] = { 0 };
  int uri_len = 0;

  if (msg->type == PJSIP_RESPONSE_MSG) {
    PRINT_COLOR(COLOR_BLUE, "SIP/2.0 ");
    PRINT_COLOR(COLOR_CYAN, "%d ", msg->line.status.code);
    PRINT_COLOR(COLOR_GREEN, "%.*s",
        (int)msg->line.status.reason.slen, msg->line.status.reason.ptr);

  } else {

    PRINT_COLOR(COLOR_BLUE, "%.*s ",
        (int)msg->line.req.method.name.slen, msg->line.req.method.name.ptr);

    uri_len = pjsip_uri_print (PJSIP_URI_IN_REQ_URI,
        msg->line.req.uri, uri_buf, 128);
    PRINT_COLOR(COLOR_CYAN, "%.*s ", uri_len, uri_buf);
    PRINT_COLOR(COLOR_GREEN, "SIP/2.0");
  }

  print_trail_chr();

  term_restore_color();
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

  PRINT_COLOR(COLOR_GREEN, "%.*s", hname_len, header); // SIP header name
  PRINT_COLOR(COLOR_RED, ":");
  PRINT_COLOR(COLOR_YELLOW, "%.*s", hval_len, hname_col + 1);
  print_trail_chr();
  term_restore_color ();
}

static void print_hdr_clid (pjsip_cid_hdr *cid)
{
  if (!cid) {
    PJ_LOG(1, (NAME, "Failed to extract Call-ID header!"));
    return;
  }
  PRINT_COLOR(COLOR_GREEN, "%.*s", (int)cid->name.slen, cid->name.ptr);
  PRINT_COLOR(COLOR_RED, ": ");
  PRINT_COLOR(COLOR_BRIGHT_BLUE, "%.*s", (int)cid->id.slen, cid->id.ptr);
  print_trail_chr();
  term_restore_color();
}

static void print_sipmsg_headers (const pjsip_msg *msg)
{
  pjsip_hdr *hdr = NULL;
  char value[ 512 ] = { 0 };

  for (hdr=msg->hdr.next; hdr!=&msg->hdr; hdr=hdr->next) {
    if (hdr->type == PJSIP_H_CALL_ID) {
      print_hdr_clid (PJSIP_MSG_CID_HDR(msg));
    } else {
      int len = hdr->vptr->print_on( hdr, value, 512 );
      print_generic_header (value, len);
    }
  }

}

/* Notification on incoming messages */
static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata)
{
  pjsip_msg *msg = rdata->msg_info.msg;

  PJ_LOG(3, (PROJECT_NAME, "RX %d bytes %s from %s %s:%d:\n",
        rdata->msg_info.len,
        pjsip_rx_data_get_info(rdata),
        rdata->tp_info.transport->type_name,
        rdata->pkt_info.src_name,
        rdata->pkt_info.src_port, (int)rdata->msg_info.len));

  print_sipmsg_head (msg);

  print_sipmsg_headers (msg);

  print_sipmsg_body (msg, PJ_FALSE);

  puts("");

  return PJ_FALSE; // continue with othe modules
}

/* Notification on outgoing messages */
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata)
{
  pjsip_msg *msg = tdata->msg;

  PJ_LOG(3, (PROJECT_NAME, "TX %d bytes %s to %s %s:%d:\n",
        (tdata->buf.cur - tdata->buf.start),
        pjsip_tx_data_get_info(tdata),
        tdata->tp_info.transport->type_name,
        tdata->tp_info.dst_name,
        tdata->tp_info.dst_port));

  print_sipmsg_head (msg);

  print_sipmsg_headers (msg);

  print_sipmsg_body (msg, PJ_TRUE);

  puts("");

  return PJ_SUCCESS; //continue with other modules
}

PJ_DEF(pj_status_t) sippak_mod_logger_register(struct sippak_app *app)
{
  ENABLE_COLORS = (app->cfg.log_decor & PJ_LOG_HAS_COLOR)
    ? PJ_TRUE
    : PJ_FALSE;

  PRINT_TRAIL_CHR = app->cfg.trail_dot;

  return pjsip_endpt_register_module(app->endpt, &msg_logger);
}
