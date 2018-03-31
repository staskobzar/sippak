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

static void set_nameservers_list (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=8.8.8.8:553,9.9.9.1" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
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
  sippak_init(&app);
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
  sippak_init(&app);
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
  sippak_init(&app);
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
  sippak_init(&app);
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
  sippak_init(&app);
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
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.log_level, 0);
}

static void one_extra_arg_is_destination (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "sip:alice@example.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_PING);
  assert_string_equal ("sip:alice@example.com", app.cfg.dest.ptr);
}

static void arg_cmd_ping_and_dest_set (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "ping", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_PING);
  assert_string_equal ("sip:bob@foo.com", app.cfg.dest.ptr);
}

static void arg_cmd_ping_and_dest_set_case_insens (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "PiNg", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_PING);
  assert_string_equal ("sip:bob@foo.com", app.cfg.dest.ptr);
}

static void arg_invalid_cmd (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "foo", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app.cfg.cmd, CMD_UNKNOWN);
  assert_string_equal ("sip:bob@foo.com", app.cfg.dest.ptr);
}

static void enable_color_argument (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--color", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_COLOR, app.cfg.log_decor & PJ_LOG_HAS_COLOR);
}

static void print_trailing_dot (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--trail-dot", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_TRUE, app.cfg.trail_dot);
}

static void log_has_time (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--log-time" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_TIME, app.cfg.log_decor & PJ_LOG_HAS_TIME);
  assert_int_equal (PJ_LOG_HAS_MICRO_SEC, app.cfg.log_decor & PJ_LOG_HAS_MICRO_SEC);
}

static void log_has_level (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--log-level" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_LEVEL_TEXT, app.cfg.log_decor & PJ_LOG_HAS_LEVEL_TEXT);
}

static void log_has_sender (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--log-snd" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_SENDER, app.cfg.log_decor & PJ_LOG_HAS_SENDER);
}

static void set_local_port_long (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--local-port=8899" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (8899, app.cfg.local_port);
}

static void set_local_port_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-P 9988" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (9988, app.cfg.local_port);
}

static void set_username_long (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--username=myuser" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("myuser", app.cfg.username.ptr);
}

static void set_username_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-u bobuser" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("bobuser", app.cfg.username.ptr);
}

static void set_local_host_long (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--local-host=127.0.0.5" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("127.0.0.5", app.cfg.local_host.ptr);
}

static void set_local_host_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-H 127.0.0.8" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("127.0.0.8", app.cfg.local_host.ptr);
}

static void set_from_name_short (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-F \"Hello World!\"" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("\"Hello World!\"", app.cfg.from_name.ptr);
}

static void set_from_name_long (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--from-name=\"Gianni Schicchi\"" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, &app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("\"Gianni Schicchi\"", app.cfg.from_name.ptr);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(pass_no_arguments_prints_usage),
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

    cmocka_unit_test(one_extra_arg_is_destination),
    cmocka_unit_test(arg_cmd_ping_and_dest_set),
    cmocka_unit_test(arg_cmd_ping_and_dest_set_case_insens),
    cmocka_unit_test(arg_invalid_cmd),

    cmocka_unit_test(enable_color_argument),
    cmocka_unit_test(print_trailing_dot),
    cmocka_unit_test(log_has_time),
    cmocka_unit_test(log_has_level),
    cmocka_unit_test(log_has_sender),

    cmocka_unit_test(set_local_port_long),
    cmocka_unit_test(set_local_port_short),
    cmocka_unit_test(set_username_long),
    cmocka_unit_test(set_username_short),
    cmocka_unit_test(set_local_host_short),
    cmocka_unit_test(set_local_host_long),
    cmocka_unit_test(set_from_name_short),
    cmocka_unit_test(set_from_name_long),
  };

  return cmocka_run_group_tests_name("Agruments parsing", tests, NULL, NULL);
}
