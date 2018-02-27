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
#include <pjlib-util.h>
#include "sippak.h"

struct pj_getopt_option sippak_long_opts[] = {
  {"help",  0,  0,  'h'},
  { NULL,   0,  0,  0  }
};

void sippak_init (struct sippak_app *app)
{
  app->cfg.log_level  = 2;
  app->cfg.cmd        = CMD_PING;
}

pj_status_t sippak_getopts (int argc, const char **argv, struct sippak_app *app)
{
  int c, opt_index;

  if (argc == 1) {

    app->cfg.cmd = CMD_HELP;
    return PJ_SUCCESS;

  }

  while ((c = pj_getopt_long(argc, argv, "h", sippak_long_opts, &opt_index)) != -1)
  {
    switch (c) {
      case 'h':
        app->cfg.cmd = CMD_HELP;
        break;
      default:
        // PJ_LOG(1, ("invalid argument %s", argv[opt_index]));
        return PJ_EINVAL;
    }
  }
  return PJ_SUCCESS;
}

