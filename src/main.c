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

#include "sippak.h"

static pj_bool_t sippak_loop_stop = PJ_FALSE;
struct sippak_app app;

static void sippak_main_loop()
{
  while (sippak_loop_stop == PJ_FALSE) {
    pj_time_val timeout = {0, 500};
    pjsip_endpt_handle_events(app.endpt, &timeout);
  }
}

PJ_DEF(void) sippak_loop_cancel()
{
  sippak_loop_stop = PJ_TRUE;
}

int main(int argc, char *argv[])
{
  pj_status_t status;
  pj_caching_pool cp;


  pj_log_set_level(MIN_LOG_LEVEL);
  status = pj_init();
  SIPPAK_ASSERT_SUCC(status, "Failed to init PJSIP.");

  status = sippak_init(&app);
  SIPPAK_ASSERT_SUCC(status, "Failed to init SIPPAK.");

  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);
  app.cp = &cp;

  status = pjsip_endpt_create(&cp.factory, PROJECT_NAME, &app.endpt);
  SIPPAK_ASSERT_SUCC(status, "Failed to create end point.");

  app.pool = pjsip_endpt_create_pool(app.endpt, PROJECT_NAME, POOL_INIT, POOL_INCR);

  status = sippak_getopts(argc, argv, &app);
  SIPPAK_ASSERT_SUCC(status, "Failed to process parameters.");

  status = sippak_mod_logger_register(&app);
  SIPPAK_ASSERT_SUCC(status, "Failed to register logger module.");

  status = sippak_set_resolver_ns (&app);
  SIPPAK_ASSERT_SUCC(status, "Failed to set DNS resolvers.");

  status = sippak_mod_sip_mangler_register (&app);
  SIPPAK_ASSERT_SUCC(status, "Failed to register SIP mangler module.");

  // run
  switch (app.cfg.cmd)
  {
    case CMD_VERSION:
      version ();
      goto done;
      break;
    case CMD_HELP:
      usage ();
      goto done;
      break;
    case CMD_PING:
      status = sippak_cmd_ping(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed PING command.");
      break;
    case CMD_PUBLISH:
      status = sippak_cmd_publish(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed PUBLISH command.");
      break;
    case CMD_SUBSCRIBE:
      status = sippak_cmd_subscribe(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed SUBSCRIBE command.");
      break;
    case CMD_NOTIFY:
      status = sippak_cmd_notify(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed NOTIFY command.");
      break;
    case CMD_REGISTER:
      status = sippak_cmd_register(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed REGISTER command.");
      break;
    case CMD_REFER:
      status = sippak_cmd_refer(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed REFER command.");
      break;
    case CMD_MESSAGE:
      status = sippak_cmd_message(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed MESSAGE command.");
      break;
    case CMD_INVITE:
      status = sippak_cmd_invite(&app);
      SIPPAK_ASSERT_SUCC(status, "Failed INVITE command.");
      break;

    // fail
    case CMD_UNKNOWN:
    default:
      PJ_LOG(1,(PROJECT_NAME, "Invalid or missing parameters. Use --help/-h for usage message."));
      status = PJ_EINVAL;
      goto done;
      break;
  }
  // main loop
  sippak_main_loop();

done:
  pj_caching_pool_destroy(&cp);

  return status;
}
