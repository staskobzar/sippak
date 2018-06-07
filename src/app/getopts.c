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
 * @file getopts.c
 * @brief sippak helper parsing cli arguments and set configuration structure.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <stdlib.h> // atoi
#include <pjlib-util.h>
#include "sippak.h"

static int is_string_numeric(const char *optarg);
static pj_str_t pjstr_trimmed(const char *optarg);
static void sippak_parse_argv_left (struct sippak_app *app, int argc, char *argv[], int pj_optind);
static void set_event (struct sippak_app *app, const char *event);
static void set_content_type (struct sippak_app *app, const char *type);
static pj_str_t pres_note (const char *note);
static pj_bool_t pres_status_open (const char *status);
static int transport_proto (const char *proto);
static int set_port_value (const char *port);
static void add_custom_header (char *header, struct sippak_app *app);
static int parse_command_str (const char *cmd);
static void set_mwi_list (struct sippak_app *app, char *mwi_list_str);
static void post_parse_setup (struct sippak_app *app);

static enum opts_enum_t {
  OPT_NS = 1,
  OPT_COLOR,
  OPT_TRAIL, // print trailing dot
  OPT_LOG_TIME, // print time and ms
  OPT_LOG_LEVEL,
  OPT_LOG_SND,
  OPT_PRES_STATUS,
  OPT_PRES_NOTE,
  OPT_MWI_ACC,
  OPT_IS_CLIST,
  OPT_CANCEL_ALL_REG,
  OPT_CANCEL,
  OPT_REFER_TO,
  OPT_BODY,
  OPT_CODEC,
  OPT_RTP_PORT
} opt_enum;

struct pj_getopt_option sippak_long_opts[] = {
  {"help",        0,  0,  'h'},
  {"version",     0,  0,  'V'},
  {"ns",          1,  0,  OPT_NS },
  {"verbose",     2,  0,  'v' },
  {"quiet",       0,  0,  'q' },
  {"color",       0,  0,  OPT_COLOR },
  {"trail-dot",   0,  0,  OPT_TRAIL },
  {"log-time",    0,  0,  OPT_LOG_TIME },
  {"log-level",   0,  0,  OPT_LOG_LEVEL },
  {"log-snd",     0,  0,  OPT_LOG_SND },
  {"local-port",  1,  0,  'P' },
  {"local-host",  1,  0,  'l' },
  {"username",    1,  0,  'u' },
  {"password",    1,  0,  'p' },
  {"from-name",   1,  0,  'F' },
  {"proto",       1,  0,  't' },
  {"expires",     1,  0,  'X' },
  {"pres-status", 1,  0,  OPT_PRES_STATUS},
  {"pres-note",   1,  0,  OPT_PRES_NOTE},
  {"content-type",1,  0,  'C' },
  {"event",       1,  0,  'E' },
  {"mwi",         1,  0,  'M' },
  {"mwi-acc",     1,  0,  OPT_MWI_ACC },
  {"clist",       0,  0,  OPT_IS_CLIST },
  {"cancel-all",  0,  0,  OPT_CANCEL_ALL_REG },
  {"cancel",      0,  0,  OPT_CANCEL},
  {"contact",     1,  0,  'c' },
  {"to",          1,  0,  OPT_REFER_TO },
  {"body",        1,  0,  OPT_BODY },
  {"codec",       1,  0,  OPT_CODEC },
  {"rtp-port",    1,  0,  OPT_RTP_PORT },
  {"user-agent",  1,  0,  'A' },
  {"header",      1,  0,  'H' },
  { NULL,         0,  0,   0  }
};

static const char *optstring = "hVvqP:u:p:t:l:F:X:E:C:M:c:A:H:";

static int parse_command_str (const char *cmd)
{
  if (pj_ansi_strnicmp(cmd, "ping", 4) == 0) {
    return CMD_PING;
  } else if (pj_ansi_strnicmp(cmd, "publish", 7) == 0) {
    return CMD_PUBLISH;
  } else if (pj_ansi_strnicmp(cmd, "subscribe", 9) == 0) {
    return CMD_SUBSCRIBE;
  } else if (pj_ansi_strnicmp(cmd, "notify", 6) == 0) {
    return CMD_NOTIFY;
  } else if (pj_ansi_strnicmp(cmd, "register", 8) == 0) {
    return CMD_REGISTER;
  } else if (pj_ansi_strnicmp(cmd, "refer", 5) == 0) {
    return CMD_REFER;
  } else if (pj_ansi_strnicmp(cmd, "message", 7) == 0) {
    return CMD_MESSAGE;
  } else if (pj_ansi_strnicmp(cmd, "invite", 6) == 0) {
    return CMD_INVITE;
  }

  return CMD_UNKNOWN;
}

static int transport_proto (const char *proto)
{
  if (pj_ansi_strnicmp(proto, "tcp", 3) == 0) {
    return PJSIP_TRANSPORT_TCP;
  }
  return PJSIP_TRANSPORT_UDP;
}

static int set_port_value (const char *port_str) {
  int port = 0;
  if(! is_string_numeric(port_str)) {
    PJ_LOG(1, (PROJECT_NAME, "Invalid port: %s.", port_str));
    exit(PJ_CLI_EINVARG);
  }
  port = atoi(port_str);
  if (port < 1 || port > ((2 << 15) - 1)) {
    PJ_LOG(1, (PROJECT_NAME, "Invalid port: %s. Expected value must be between 1 and %d.",
          port_str, (2 << 15) - 1));
    exit(PJ_CLI_EINVARG);
  }

  return port;
}

static void add_custom_header (char *header, struct sippak_app *app)
{
  pj_str_t n = pj_str("Subject");
  pj_str_t v = pj_str("Ping Pong");
  app->cfg.hdrs.h[0] = pjsip_generic_string_hdr_create(app->pool, &n, &v);
  app->cfg.hdrs.cnt++;
}

static pj_bool_t pres_status_open (const char *status)
{
  if (pj_ansi_strnicmp(status, "closed", 6) == 0) {
    return PJ_FALSE;
  } else if (pj_ansi_strnicmp(status, "open", 4) == 0) {
    return PJ_TRUE;
  } else {
    PJ_LOG(2, (PROJECT_NAME, "Unknown presence status '%s'. Will use 'open'.", status));
    return PJ_TRUE;
  }
}

static pj_str_t pres_note (const char *note)
{
  pj_str_t val = {NULL, 0};
  val = pj_str (pj_optarg);
  pj_strtrim(&val);
  if (val.slen > MAX_LEN_PRES_NOTE) {
    val.slen = MAX_LEN_PRES_NOTE;
    PJ_LOG(2, (PROJECT_NAME,
          "Note message is too long. Max %d char allowed. Message is stripped.",
          MAX_LEN_PRES_NOTE));
  }
  return val;
}

static void set_content_type (struct sippak_app *app,
                              const char *type)
{
  pj_ssize_t found_idx = 0;
  pj_str_t token;
  pj_str_t value = pjstr_trimmed(type);
  pj_str_t delim = pj_str("/");

  if (pj_strnicmp2(&value, "pidf", 4) == 0) {

    app->cfg.ctype_media.type = pj_str("application");
    app->cfg.ctype_media.subtype = pj_str("pidf+xml");
    app->cfg.ctype_e = CTYPE_PIDF;

  } else if (pj_strnicmp2(&value, "xpidf", 5) == 0) {

    app->cfg.ctype_media.type = pj_str("application");
    app->cfg.ctype_media.subtype = pj_str("xpidf+xml");
    app->cfg.ctype_e = CTYPE_XPIDF;

  } else if (pj_strnicmp2(&value, "mwi", 3) == 0) {

    app->cfg.ctype_media.type = pj_str("application");
    app->cfg.ctype_media.subtype = pj_str("simple-message-summary");
    app->cfg.ctype_e = CTYPE_MWI;

  } else {
    if (pj_strstr(&value, &delim) == NULL) {
      app->cfg.ctype_media.type = pj_str("application");
      app->cfg.ctype_media.subtype = value;
    } else {
      found_idx = pj_strtok(&value, &delim, &token, 0);
      app->cfg.ctype_media.type = token;
      pj_strtok(&value, &delim, &token, found_idx + token.slen);
      app->cfg.ctype_media.subtype = token;
    }
    app->cfg.ctype_e = CTYPE_OTHER;
  }
}

static void set_mwi_list (struct sippak_app *app, char *mwi_list_str)
{
  pj_ssize_t found_idx = 0;
  int mwi_idx = 0;
  pj_str_t token;
  pj_str_t delim = pj_str(",");
  pj_str_t list = pj_str(mwi_list_str);

  // init array
  for (int i = 0; i < 4; i++) app->cfg.mwi[i] = 0;

  // parse list
  for (
      found_idx = pj_strtok (&list, &delim, &token, 0);
      found_idx != list.slen && mwi_idx < 4;
      found_idx = pj_strtok (&list, &delim, &token, found_idx + token.slen)
      )
  {
    app->cfg.mwi[mwi_idx++] = pj_strtol(&token);
  }

  app->cfg.is_mwi = PJ_TRUE;
}

static void set_event (struct sippak_app *app, const char *event)
{
  pj_str_t value = pjstr_trimmed(event);
  if (pj_strnicmp2(&value, "presence", 8) == 0) {
    app->cfg.pres_ev = EVTYPE_PRES;
  } else if (pj_strnicmp2(&value, "mwi", 3) == 0 ||
             pj_strnicmp2(&value, "message-summary", 15) == 0) {
    app->cfg.pres_ev = EVTYPE_MWI;
  } else {
    app->cfg.pres_ev = EVTYPE_OTHER;
  }

  app->cfg.event = value;
}

static void sippak_parse_argv_left (struct sippak_app *app,
                                    int argc, char *argv[],
                                    int pj_optind)
{
  int idx = pj_optind;

  if (idx == argc) {
    return; // no command or destination is given
  }

  if (1 == (argc - idx)) {
    // only one arg left. Consider it is a destination.
    app->cfg.dest = pj_str(argv[idx]);
    return; // assign destination and exit
  }

  // command and destination are provided
  // set command
  app->cfg.cmd = parse_command_str (argv[idx]);

  // set destination
  app->cfg.dest = pj_str(argv[idx + 1]);

  if (argc > (idx + 2)) {
    PJ_LOG(3, (PROJECT_NAME, "Extra arguments will be skipped"));
  }
}

static pj_str_t pjstr_trimmed(const char *optarg)
{
  pj_str_t val = {NULL, 0};
  val = pj_str (pj_optarg);
  pj_strtrim(&val);
  return val;
}

static int is_string_numeric(const char *optarg)
{
  int i;
  pj_str_t val = {NULL, 0};
  val = pj_str (pj_optarg);
  pj_strtrim(&val);
  for (i = 0; i < val.slen; i++) {
    if (val.ptr[i] < '0' || val.ptr[i] > '9') {
      return 0;
    }
  }
  return 1;
}

pj_status_t sippak_init (struct sippak_app *app)
{
  // init main application structure
  app->cfg.dest.slen        = 0;
  app->cfg.dest.ptr         = NULL;
  app->cfg.log_level        = MIN_LOG_LEVEL;
  app->cfg.cmd              = CMD_PING;
  app->cfg.nameservers      = NULL;
  app->cfg.log_decor        = PJ_LOG_HAS_NEWLINE | PJ_LOG_HAS_INDENT;
  app->cfg.trail_dot        = PJ_FALSE;
  app->cfg.local_port       = 0;
  app->cfg.local_host.ptr   = NULL;
  app->cfg.local_host.slen  = 0;
  app->cfg.username.ptr     = NULL;
  app->cfg.username.slen    = 0;
  app->cfg.password.ptr     = NULL;
  app->cfg.password.slen    = 0;
  app->cfg.from_name.ptr    = NULL;
  app->cfg.from_name.slen   = 0;
  app->cfg.proto            = PJSIP_TRANSPORT_UDP;
  app->cfg.expires          = 3600;
  app->cfg.pres_status_open = PJ_TRUE;
  app->cfg.pres_note.ptr    = NULL;
  app->cfg.pres_note.slen   = 0;
  app->cfg.ctype_e          = CTYPE_UNKNOWN;
  app->cfg.pres_ev          = EVTYPE_UNKNOWN;
  app->cfg.event.slen       = 0;
  app->cfg.event.ptr        = NULL;
  app->cfg.mwi_acc.slen     = 0;
  app->cfg.mwi_acc.ptr      = NULL;
  app->cfg.is_mwi           = PJ_FALSE;
  app->cfg.is_clist         = PJ_FALSE;
  app->cfg.cancel_all_reg   = PJ_FALSE;
  app->cfg.cancel           = PJ_FALSE;
  app->cfg.contact.slen     = 0;
  app->cfg.contact.ptr      = NULL;
  app->cfg.refer_to.slen    = 0;
  app->cfg.refer_to.ptr     = NULL;
  app->cfg.body.slen        = 0;
  app->cfg.body.ptr         = NULL;

  // default codec is g711
  app->cfg.media.cnt        = 1;
  app->cfg.media.codec[0]   = SIPPAK_CODEC_G711;
  app->cfg.media.rtp_port   = SIPPAK_DEFAULT_RTP_PORT;

  app->cfg.user_agent.slen  = 0;
  app->cfg.user_agent.ptr   = NULL;

  // custom headers
  app->cfg.hdrs.cnt        = 0;

  return PJ_SUCCESS;
}

static void post_parse_setup (struct sippak_app *app)
{
  // set log decoration and level
  pj_log_set_decor(app->cfg.log_decor);
  pj_log_set_level(app->cfg.log_level);

  // command REFER requires refer-to parameter
  if (app->cfg.cmd == CMD_REFER && app->cfg.refer_to.ptr == NULL) {
    PJ_LOG(1, (PROJECT_NAME,
          "Failed. Requires parameter \"--to\" to initiate REFER request."));
    exit(PJ_CLI_EINVARG);
  }

  // command MESSAGE requires body parameter
  if (app->cfg.cmd == CMD_MESSAGE && app->cfg.body.ptr == NULL) {
    PJ_LOG(1, (PROJECT_NAME,
          "Failed. Requires parameter \"--body\" to initiate MESSAGE."));
    exit(PJ_CLI_EINVARG);
  }

  // reset event and presence to follow MWI routin
  if (app->cfg.is_mwi == PJ_TRUE) {
    app->cfg.pres_ev = EVTYPE_MWI;
    app->cfg.ctype_media.type = pj_str("application");
    app->cfg.ctype_media.subtype = pj_str("simple-message-summary");
    app->cfg.ctype_e = CTYPE_MWI;
    if (app->cfg.mwi_acc.slen == 0) {
      app->cfg.mwi_acc = app->cfg.dest;
    }
  }

  // default username is user part of destination URI
  if (app->cfg.username.ptr == NULL && app->cfg.dest.ptr != NULL) {
    pjsip_sip_uri *dest_uri = (pjsip_sip_uri*)pjsip_parse_uri(app->pool, app->cfg.dest.ptr,
                          app->cfg.dest.slen, 0);
    if (dest_uri == NULL) {
      PJ_LOG(1, (PROJECT_NAME, "Failed to parse destination URI: %.*s",
            app->cfg.dest.slen, app->cfg.dest.ptr));
      exit(PJ_CLI_EINVARG);
    }
    app->cfg.username = dest_uri->user;
  }
}

pj_status_t sippak_getopts (int argc, char *argv[], struct sippak_app *app)
{
  int c = 0, opt_index = 0;
  /*
   * Member of getopts.c. Need to reset this to make
   * sure multiple calls from unit tests work fine.
   */
  pj_optind = 0;

  if (argc == 1) {

    app->cfg.cmd = CMD_HELP;
    return PJ_SUCCESS;

  }

  while ((c = pj_getopt_long (argc, argv, optstring, sippak_long_opts, &opt_index)) != -1)
  {
    switch (c) {
      case 'h':
        app->cfg.cmd = CMD_HELP;
        return PJ_SUCCESS;
        break;
      case 'V':
        app->cfg.cmd = CMD_VERSION;
        return PJ_SUCCESS;
        break;
      case OPT_NS:
        app->cfg.nameservers = pj_optarg;
        break;
      case OPT_COLOR:
        app->cfg.log_decor |= PJ_LOG_HAS_COLOR;
        break;
      case OPT_TRAIL:
        app->cfg.trail_dot = PJ_TRUE;
        break;
      case OPT_LOG_TIME:
        app->cfg.log_decor |= PJ_LOG_HAS_TIME | PJ_LOG_HAS_MICRO_SEC;
        break;
      case OPT_LOG_LEVEL:
        app->cfg.log_decor |= PJ_LOG_HAS_LEVEL_TEXT;
        break;
      case OPT_LOG_SND:
        app->cfg.log_decor |= PJ_LOG_HAS_SENDER;
        break;
      case 'P':
        app->cfg.local_port = set_port_value (pj_optarg);
        break;
      case 'l':
        app->cfg.local_host = pjstr_trimmed(pj_optarg);
        break;
      case 'u':
        app->cfg.username = pjstr_trimmed(pj_optarg);
        break;
      case 'p':
        app->cfg.password = pjstr_trimmed(pj_optarg);
        break;
      case 'F':
        app->cfg.from_name = pjstr_trimmed(pj_optarg);
        break;
      case 't':
        app->cfg.proto = transport_proto (pj_optarg);
        break;
      case 'v':
        if (pj_optarg) {
          app->cfg.log_level += atoi (pj_optarg);
        } else {
          app->cfg.log_level++;
        }
        break;
      case 'q':
          app->cfg.log_level = 0;
        break;
      case 'X':
          if(!is_string_numeric(pj_optarg)) {
            PJ_LOG(1, (PROJECT_NAME, "Invalid expires value: %s. Must be numeric.", pj_optarg));
            exit(PJ_CLI_EINVARG);
          }
          app->cfg.expires = atoi(pj_optarg);
        break;
      case OPT_PRES_STATUS:
        app->cfg.pres_status_open = pres_status_open (pj_optarg);
        break;
      case OPT_PRES_NOTE:
        app->cfg.pres_note = pres_note (pj_optarg);
        break;
      case 'C': // content type
        set_content_type(app, pj_optarg);
        break;
      case 'E': // event
        set_event(app, pj_optarg);
        break;
      case 'M': // event
        set_mwi_list(app, pj_optarg);
        break;
      case OPT_MWI_ACC: // event
        app->cfg.mwi_acc = pjstr_trimmed(pj_optarg);
        break;
      case OPT_IS_CLIST:
        app->cfg.is_clist = PJ_TRUE;
        break;
      case OPT_CANCEL_ALL_REG:
        app->cfg.cancel_all_reg = PJ_TRUE;
        break;
      case OPT_CANCEL:
        app->cfg.cancel = PJ_TRUE;
        break;
      case 'c': // custom contact header
        app->cfg.contact = pjstr_trimmed(pj_optarg);
        break;
      case OPT_REFER_TO:
        app->cfg.refer_to = pjstr_trimmed(pj_optarg);
        break;
      case OPT_BODY:
        app->cfg.body = pjstr_trimmed(pj_optarg);
        break;
      case OPT_CODEC:
        if (sippak_set_media_codecs_cfg(pj_optarg, app) != PJ_SUCCESS) {
          exit(PJ_CLI_EINVARG);
        }
        break;
      case OPT_RTP_PORT:
        app->cfg.media.rtp_port = set_port_value(pj_optarg);
        break;
      case 'A': // User-Agent header
        app->cfg.user_agent = pjstr_trimmed(pj_optarg);
        break;
      case 'H': // Add custom header
        add_custom_header (pj_optarg, app);
        break;
      default:
        break;
    }
  }

  // get command and destination

  sippak_parse_argv_left (app, argc, argv, pj_optind);

  // finilize the setup
  post_parse_setup(app);

  return PJ_SUCCESS;
}

