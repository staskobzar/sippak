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
  sippak_app_cfg_init(&app);
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
  sippak_app_cfg_init(&app);
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
  sippak_app_cfg_init(&app);
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
  sippak_app_cfg_init(&app);
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
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_VERSION);
}

static void set_nameservers_list (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=8.8.8.8:553,9.9.9.1" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("8.8.8.8:553,9.9.9.1", app.cfg.nameservers);
}

static void set_verbosity_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-v" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, MIN_LOG_LEVEL + 1);
}

static void set_verbosity_multy_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-vvvv" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, MIN_LOG_LEVEL + 4);
}

static void set_verbosity_long_no_val (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--verbose" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, MIN_LOG_LEVEL + 1);
}

static void set_verbosity_long_with_val (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--verbose=6" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, MIN_LOG_LEVEL + 6);
}

static void suppress_verbosity_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-q" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, 0);
}

static void suppress_verbosity_long (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--quiet" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_app_cfg_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, 0);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(pass_no_arguments_prints_usage),
    // cmocka_unit_test(pass_invalid_arguments),
    cmocka_unit_test(pass_help_long_opt),
    cmocka_unit_test(pass_help_short_opt),
    cmocka_unit_test(pass_version_long_opt),
    cmocka_unit_test(pass_version_short_opt),
    cmocka_unit_test(set_nameservers_list),
    cmocka_unit_test(set_verbosity_short),
    cmocka_unit_test(set_verbosity_multy_short),
    cmocka_unit_test(set_verbosity_long_no_val),
    cmocka_unit_test(set_verbosity_long_with_val),
    cmocka_unit_test(suppress_verbosity_short),
    cmocka_unit_test(suppress_verbosity_long),
  };
  return cmocka_run_group_tests_name("Agruments parsing", tests, NULL, NULL);
}
