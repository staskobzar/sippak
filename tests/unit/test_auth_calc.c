#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

struct test_app {
  pj_pool_t *pool;
  pj_caching_pool cp;
  pjsip_endpoint *endpt;
};

static int setup_app(void **state)
{
  struct test_app *app = malloc(sizeof(struct test_app));
  pj_init ();
  pj_caching_pool_init(&app->cp, &pj_pool_factory_default_policy, 0);
  pjsip_endpt_create(&app->cp.factory, PROJECT_NAME, &app->endpt);
  // app->pool = pj_pool_create( &app->cp.factory, "test_auth_calc", 4000, 4000, NULL);
  app->pool = pjsip_endpt_create_pool(app->endpt, "test_auth_calc", POOL_INIT, POOL_INCR);
  *state = app;
  return 0;
}

static int teardown_app(void **state)
{
  struct test_app *app = *state;
  pjsip_endpt_destroy(app->endpt);
  pj_pool_release( app->pool );
  pj_caching_pool_destroy( &app->cp );
  free(app);
  pj_shutdown();
  return 0;
}

static void cmd_auth_calc_set (void **state)
{
  (void) *state;
  struct sippak_app app;
  char auth_header[] = "'WWW-Authenticate: Digest realm=\"sip.net\", nonce=\"5aed68\"'";
  char *argv[] = { "./sippak", "authcalc", auth_header };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  assert_int_equal (app.cfg.cmd, CMD_AUTHCALC);
  assert_string_equal (auth_header, app.cfg.auth_header.ptr);
}

static void cmd_auth_calc_www_header (void **state)
{
  pj_pool_t *pool;
  pj_caching_pool cp;
  pjsip_endpoint *endpt;
  pj_init ();
  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);
  pjsip_endpt_create(&cp.factory, PROJECT_NAME, &endpt);
  // app->pool = pj_pool_create( &app->cp.factory, "test_auth_calc", 4000, 4000, NULL);
  pool = pjsip_endpt_create_pool(endpt, "test_auth_calc", POOL_INIT, POOL_INCR);

  struct sippak_app app;
  // example from RFC2617
  char auth_header[] = "WWW-Authenticate: Digest realm=\"testrealm@host.com\","
    "qop=\"auth,auth-int\","
    "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
    "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"";

  char *argv[] = { "./sippak", "authcalc", auth_header };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  app.pool = pool;
  sippak_getopts (argc, argv, &app);
  assert_int_equal (app.cfg.cmd, CMD_AUTHCALC);
  assert_string_equal (auth_header, app.cfg.auth_header.ptr);
  sippak_auth_calc (&app);

  pj_pool_release( pool );
  pj_caching_pool_destroy( &cp );
  pj_shutdown();
}

int main(int argc, const char *argv[])
{
  pj_log_set_level(0); // do not print pj debug on init
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(cmd_auth_calc_set),
    //cmocka_unit_test_setup_teardown (cmd_auth_calc_www_header, setup_app, teardown_app),
    cmocka_unit_test(cmd_auth_calc_www_header),
  };
  return cmocka_run_group_tests_name("Auth header calc", tests, NULL, NULL);
}
