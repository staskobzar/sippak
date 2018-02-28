#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

static void pass_no_arguments_prints_usage (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_HELP);
}

static void pass_help_short_opt (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-h" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_HELP);
}

static void pass_help_long_opt (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--help" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_HELP);
}

static void pass_version_short_opt (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-V" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_VERSION);
}

static void pass_version_long_opt (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--version" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_VERSION);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(pass_no_arguments_prints_usage),
    cmocka_unit_test(pass_help_long_opt),
    cmocka_unit_test(pass_help_short_opt),
    cmocka_unit_test(pass_version_long_opt),
    cmocka_unit_test(pass_version_short_opt),
  };
  return cmocka_run_group_tests_name("Agruments parsing", tests, NULL, NULL);
}
