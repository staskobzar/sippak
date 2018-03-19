#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

static void cmd_auth_calc (void **state)
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

int main(int argc, const char *argv[])
{
  pj_log_set_level(0); // do not print pj debug on init
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(cmd_auth_calc),
  };
  return cmocka_run_group_tests_name("Auth header calc", tests, NULL, NULL);
}
