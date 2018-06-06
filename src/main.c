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

void sippak_loop_cancel()
{
  sippak_loop_stop = PJ_TRUE;
}

int main(int argc, char *argv[])
{
  pj_status_t status;
  pj_caching_pool cp;

  pj_log_set_level(MIN_LOG_LEVEL);
  status = pj_init();
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);

  status = sippak_init(&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);
  app.cp = &cp;

  status = pjsip_endpt_create(&cp.factory, PROJECT_NAME, &app.endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  app.pool = pjsip_endpt_create_pool(app.endpt, PROJECT_NAME, POOL_INIT, POOL_INCR);

  status = sippak_getopts(argc, argv, &app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = sippak_mod_logger_register(&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = sippak_set_resolver_ns (&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  status = sippak_mod_sip_mangler_register (&app);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

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
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_PUBLISH:
      status = sippak_cmd_publish(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_SUBSCRIBE:
      status = sippak_cmd_subscribe(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_NOTIFY:
      status = sippak_cmd_notify(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_REGISTER:
      status = sippak_cmd_register(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_REFER:
      status = sippak_cmd_refer(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_MESSAGE:
      status = sippak_cmd_message(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
      break;
    case CMD_INVITE:
      status = sippak_cmd_invite(&app);
      PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
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
