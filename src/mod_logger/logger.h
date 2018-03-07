#ifndef __MOD_LOGGER_LOGGER_H
#define __MOD_LOGGER_LOGGER_H

static pj_bool_t ENABLE_COLORS = PJ_FALSE;
static char TRAIL_CHR = '.'; // end of line
static pj_bool_t PRINT_TRAIL_CHR = PJ_FALSE;

static void term_set_color(int level);
static void term_restore_color(void);
static void print_sipmsg_body(pjsip_msg_body *body);
static void print_sipmsg_head(pjsip_msg *msg);
static void print_generic_header (const char *header, int len);
static void print_trail_chr ();

static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata);
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata);
#endif
