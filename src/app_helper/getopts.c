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

void sippak_app_cfg_init (struct sippak_app *app)
{
  // init main application structure
  app->cfg.log_level    = MIN_LOG_LEVEL;
  app->cfg.cmd          = CMD_PING;
  app->cfg.nameservers  = NULL;
}

pj_status_t sippak_init (struct sippak_app *app)
{
  pj_status_t status;
  pjsip_endpoint *endpt;
  pj_pool_t *pool;
  pj_caching_pool cp;

  pj_log_set_level(0); // do not print pj debug on init

  status = pj_init();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);

  status = pjsip_endpt_create(&cp.factory, PROJECT_NAME, &endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pool = pjsip_endpt_create_pool(endpt, PROJECT_NAME, POOL_INIT, POOL_INCR);

  app->endpt = endpt;
  app->pool  = pool;
  app->cp    = &cp;

  sippak_app_cfg_init (app);

  return status;
}

pj_status_t sippak_getopts (int argc, char *argv[], struct sippak_app *app)
{
  int c = 0, opt_index = 0;
  /*
   * Member of getopts.c. Need to reset this to make
   * sure multiple calls from unit tests work fine.
   */
  pj_optind = 1;

  if (argc == 1) {

    app->cfg.cmd = CMD_HELP;
    return PJ_SUCCESS;

  }

  while ((c = pj_getopt_long (argc, argv, "hVvq", sippak_long_opts, &opt_index)) != -1)
  {
    switch (c) {
      case 'h':
        app->cfg.cmd = CMD_HELP;
        break;
      case 'V':
        app->cfg.cmd = CMD_VERSION;
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
  return PJ_SUCCESS;
}

