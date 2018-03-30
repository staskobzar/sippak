#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

static void cmd_auth_calc_set (void **state)
{
  (void) *state;
  struct sippak_app app;
  char *argv[] = { "./sippak", "authcalc", "--nonce=dcd98b", "sip:example.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  assert_int_equal (app.cfg.cmd, CMD_AUTHCALC);
  assert_string_equal ("sip:example.com", app.cfg.dest.ptr);
  assert_string_equal ("dcd98b", app.cfg.nonce.ptr);
  assert_string_equal ("REGISTER", app.cfg.method.ptr);
  assert_string_equal ("alice", app.cfg.username.ptr);

  sippak_auth_calc(&app);
}

static void cmd_auth_calc_www_header (void **state)
{
  struct sippak_app app;
  // example from RFC2617
  char auth_header[] = "WWW-Authenticate: Digest realm=\"testrealm@host.com\","
    "qop=\"auth,auth-int\","
    "nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\","
    "opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"";
}

int main(void)
{
  pj_log_set_level(0); // do not print pj debug on init
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(cmd_auth_calc_set),
    cmocka_unit_test(cmd_auth_calc_www_header),
  };
  return cmocka_run_group_tests_name("Auth header calc", tests, NULL, NULL);
}
