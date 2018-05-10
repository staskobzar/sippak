#ifndef __MOD_LOGGER_LOGGER_H
#define __MOD_LOGGER_LOGGER_H

#define SIPMSG_BODY_LEN 2048

static void term_set_color(int level);
static void term_restore_color(void);

#define PRINT_COLOR(color, ...) term_set_color (color); printf(__VA_ARGS__);

#define COLOR_BRIGHT_WHITE PJ_TERM_COLOR_BRIGHT | \
                           PJ_TERM_COLOR_R |      \
                           PJ_TERM_COLOR_G |      \
                           PJ_TERM_COLOR_B

#define COLOR_BLUE    PJ_TERM_COLOR_B
#define COLOR_CYAN    PJ_TERM_COLOR_G | PJ_TERM_COLOR_B
#define COLOR_GREEN   PJ_TERM_COLOR_G
#define COLOR_RED     PJ_TERM_COLOR_R
#define COLOR_YELLOW  PJ_TERM_COLOR_R | PJ_TERM_COLOR_G
#define COLOR_BRIGHT_BLUE PJ_TERM_COLOR_BRIGHT | PJ_TERM_COLOR_B | PJ_TERM_COLOR_R

static pj_bool_t ENABLE_COLORS = PJ_FALSE;
static char TRAIL_CHR = '.'; // end of line
static pj_bool_t PRINT_TRAIL_CHR = PJ_FALSE;

static void print_sipmsg_head (pjsip_msg *msg);
static void print_sipmsg_headers (const pjsip_msg *msg);
static void print_generic_header (const char *header, int len);
static void print_sipmsg_body (pjsip_msg *msg);
static void print_hdr_clid (pjsip_cid_hdr *cid);
static void print_trail_chr ();

static pj_bool_t logging_on_rx_msg(pjsip_rx_data *rdata);
static pj_status_t logging_on_tx_msg(pjsip_tx_data *tdata);

#endif
