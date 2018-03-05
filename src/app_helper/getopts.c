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

#define OPT_NS    1

struct pj_getopt_option sippak_long_opts[] = {
  {"help",      0,  0,  'h'},
  {"version",   0,  0,  'V'},
  {"ns",        1,  0,  OPT_NS },
  {"verbose",   2,  0,  'v' },
  {"quiet",     0,  0,  'q' },
  { NULL,       0,  0,   0 }
};

static int parse_command_str (const char *cmd)
{
  if (pj_ansi_strnicmp(cmd, "ping", 4) == 0) {
    return CMD_PING;
  }

  return CMD_UNKNOWN;
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
    app->cfg.dest = argv[idx];
    return; // assign destination and exit
  }

  // command and destination are provided
  // set command
  app->cfg.cmd = parse_command_str (argv[idx]);

  // set destination
  app->cfg.dest = argv[idx + 1];

  if (argc > (idx + 2)) {
    PJ_LOG(3, (PROJECT_NAME, "Extra arguments will be skipped"));
  }
}

pj_status_t sippak_init (struct sippak_app *app)
{
  // init main application structure
  app->cfg.log_level    = MIN_LOG_LEVEL;
  app->cfg.cmd          = CMD_PING;
  app->cfg.nameservers  = NULL;
  app->cfg.dest         = NULL;

  pj_log_set_decor(
      // PJ_LOG_HAS_TIME |
      // PJ_LOG_HAS_MICRO_SEC |
      // PJ_LOG_HAS_SENDER |
      PJ_LOG_HAS_NEWLINE |
      // PJ_LOG_HAS_SPACE |
      PJ_LOG_HAS_COLOR |
      PJ_LOG_HAS_THREAD_SWC |
      // PJ_LOG_HAS_INDENT |
      0
      );
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

  while ((c = pj_getopt_long (argc, argv, "hVvq", sippak_long_opts, &opt_index)) != -1)
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
      default:
        // PJ_LOG(1, ("invalid argument %s", argv[opt_index]));
        return PJ_EINVAL;
    }
  }

  // get command and destination
  sippak_parse_argv_left (app, argc, argv, pj_optind);

  return PJ_SUCCESS;
}

