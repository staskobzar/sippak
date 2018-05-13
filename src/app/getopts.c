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

static enum opts_enum_t {
  OPT_NS = 1,
  OPT_COLOR,
  OPT_TRAIL, // print trailing dot
  OPT_LOG_TIME, // print time and ms
  OPT_LOG_LEVEL,
  OPT_LOG_SND,
  OPT_PRES_STATUS,
  OPT_PRES_NOTE
} opt_enum;

struct pj_getopt_option sippak_long_opts[] = {
  {"help",      0,  0,  'h'},
  {"version",   0,  0,  'V'},
  {"ns",        1,  0,  OPT_NS },
  {"verbose",   2,  0,  'v' },
  {"quiet",     0,  0,  'q' },
  {"color",     0,  0,  OPT_COLOR },
  {"trail-dot", 0,  0,  OPT_TRAIL },
  {"log-time",  0,  0,  OPT_LOG_TIME },
  {"log-level", 0,  0,  OPT_LOG_LEVEL },
  {"log-snd",   0,  0,  OPT_LOG_SND },
  {"local-port",1,  0,  'P' },
  {"local-host",1,  0,  'H' },
  {"username",  1,  0,  'u' },
  {"password",  1,  0,  'p' },
  {"from-name", 1,  0,  'F' },
  {"proto",     1,  0,  't' },
  {"expires",   1,  0,  'E' },
  {"pres-status",1, 0,  OPT_PRES_STATUS},
  {"pres-note", 1,  0,  OPT_PRES_NOTE},
  { NULL,       0,  0,   0 }
};

static const char *optstring = "hVvqP:u:p:t:H:F:E:";

static int parse_command_str (const char *cmd)
{
  if (pj_ansi_strnicmp(cmd, "ping", 4) == 0) {
    return CMD_PING;
  } else if (pj_ansi_strnicmp(cmd, "publish", 7) == 0) {
    return CMD_PUBLISH;
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
  int i = 0;
  pj_str_t val = {NULL, 0};
  val = pj_str (pj_optarg);
  pj_strtrim(&val);
  for (i; i < val.slen; i++) {
    if (val.ptr[i] < '0' || val.ptr[i] > '9') {
      return 0;
    }
  }
  return 1;
}

pj_status_t sippak_init (struct sippak_app *app)
{
  // init main application structure
  app->cfg.log_level        = MIN_LOG_LEVEL;
  app->cfg.cmd              = CMD_PING;
  app->cfg.nameservers      = NULL;
  app->cfg.log_decor        = PJ_LOG_HAS_NEWLINE | PJ_LOG_HAS_INDENT;
  app->cfg.trail_dot        = PJ_FALSE;
  app->cfg.local_port       = 0;
  app->cfg.local_host.ptr   = NULL;
  app->cfg.local_host.slen  = 0;
  app->cfg.username         = pj_str("alice");
  app->cfg.password.ptr     = NULL;
  app->cfg.password.slen    = 0;
  app->cfg.from_name.ptr    = NULL;
  app->cfg.from_name.slen   = 0;
  app->cfg.proto            = PJSIP_TRANSPORT_UDP;
  app->cfg.expires          = 3600;
  app->cfg.pres_status_open = PJ_TRUE;
  app->cfg.pres_note.ptr    = NULL;
  app->cfg.pres_note.slen   = 0;

  return PJ_SUCCESS;
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
        app->cfg.local_port = atoi (pj_optarg);
        break;
      case 'H':
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
      case 'E':
          if(!is_string_numeric(pj_optarg)) {
            PJ_LOG(1, (PROJECT_NAME, "Invalid expires value: %s. Must be numeric.", pj_optarg));
            exit(PJ_CLI_EINVARG);
          }
          app->cfg.expires = atoi(pj_optarg);
          if (app->cfg.expires < 1) {
            PJ_LOG(1, (PROJECT_NAME, "Expires header value must be more then 0."));
            exit(PJ_CLI_EINVARG);
          }
        break;
      case OPT_PRES_STATUS:
        app->cfg.pres_status_open = pres_status_open (pj_optarg);
        break;
      case OPT_PRES_NOTE:
        app->cfg.pres_note = pres_note (pj_optarg);
        break;
      default:
        break;
    }
  }

  // get command and destination
  sippak_parse_argv_left (app, argc, argv, pj_optind);

  // set log decoration
  pj_log_set_decor(app->cfg.log_decor);

  return PJ_SUCCESS;
}

