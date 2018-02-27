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
 * @file main.c
 * @brief SIP command line utility main file.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include <pjsip.h>
#include <pjlib.h>

#include "sippak.h"

static pj_bool_t sippak_loop_stop = PJ_FALSE;
struct sippak_app app;

static void sippak_main_loop()
{
  for (;;) {
    pjsip_endpt_handle_events(app.endpt, NULL);
    if (sippak_loop_stop)
      break;
  }
}

void sippak_loop_cancel()
{
  sippak_loop_stop = PJ_TRUE;
}

int main(int argc, const char *argv[])
{
  pj_status_t status;
  pj_caching_pool cp;

  sippak_init(&app);

  status = sippak_getopts(argc, argv, &app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pj_log_set_level(app.cfg.log_level);

  status = pj_init();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);

  status = pjsip_endpt_create(&cp.factory, "sippak", &app.endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  app.pool = pjsip_endpt_create_pool(app.endpt, "sippak", POOL_INIT, POOL_INCR);

  status = sippak_mod_logger_register(&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = sippak_set_resolver_ns (&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = sippak_cmd_ping(&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  // main loop
  sippak_main_loop();

  pj_caching_pool_destroy(&cp);

  return 0;
}
