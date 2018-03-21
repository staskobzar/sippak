#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

struct test_app {
  pj_pool_t *pool;
  pj_caching_pool cp;
};

static int setup_app(void **state)
{
  struct test_app *app = malloc(sizeof(struct test_app));
  pj_caching_pool_init(&app->cp, &pj_pool_factory_default_policy, 0);
  app->pool = pj_pool_create( &app->cp.factory, "test_auth_calc", 4000, 4000, NULL);
  *state = app;
  return 0;
}

static int teardown_app(void **state)
{
  struct test_app *app = *state;
  pj_pool_release( app->pool );
  pj_caching_pool_destroy( &app->cp );
  free(app);
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
  struct sippak_app app;
  // example from RFC2617
  char auth_header[] = "WWW-Authenticate: Digest realm=\"testrealm@host.com\","
    "qop=\"auth,auth-int\","
    "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
    "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"";

  char *argv[] = { "./sippak", "authcalc", auth_header };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  app.pool = ((struct test_app*)*state)->pool;
  sippak_getopts (argc, argv, &app);
  assert_int_equal (app.cfg.cmd, CMD_AUTHCALC);
  assert_string_equal (auth_header, app.cfg.auth_header.ptr);
  sippak_auth_calc (&app);
}

int main(int argc, const char *argv[])
{
  pj_log_set_level(10); // do not print pj debug on init
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(cmd_auth_calc_set),
    cmocka_unit_test_setup_teardown (cmd_auth_calc_www_header, setup_app, teardown_app),
  };
  return cmocka_run_group_tests_name("Auth header calc", tests, NULL, NULL);
}
