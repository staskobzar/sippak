#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdlib.h>
#include <string.h>
#include "sippak.h"

pjsip_endpoint *endpt;
pj_pool_t *pool;

/*
 * ********** SETUP/TEARDOWN ********
 */
static int setup_app(void **state)
{
  struct sippak_app *app = malloc(sizeof(struct sippak_app));

  sippak_init(app);

  app->pool = pool;

  *state = app;
  return 0;
}

static int teardown_app(void **state)
{
  free(*state);
  return 0;
}

/*
 * ********** TESTS ********
 */
static void pass_no_arguments_prints_usage (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak" };
  int argc = sizeof(argv) / sizeof(char*);
  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_HELP);
}

static void pass_help_short_opt (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-h" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_HELP);
}

static void pass_help_long_opt (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--help" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_HELP);
}

static void pass_version_short_opt (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-V" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_VERSION);
}

static void pass_version_long_opt (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--version" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_VERSION);
}

static void set_nameservers_list (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--ns=8.8.8.8:553,9.9.9.1" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("8.8.8.8:553,9.9.9.1", app->cfg.nameservers);
}

static void set_verbosity_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-v" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.log_level, MIN_LOG_LEVEL + 1);
}

static void set_verbosity_multy_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-vvvv" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.log_level, MIN_LOG_LEVEL + 4);
}

static void set_verbosity_long_no_val (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--verbose" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.log_level, MIN_LOG_LEVEL + 1);
}

static void set_verbosity_long_with_val (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--verbose=6" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.log_level, MIN_LOG_LEVEL + 6);
}

static void suppress_verbosity_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-q" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.log_level, 0);
}

static void suppress_verbosity_long (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--quiet" };
  int argc = sizeof(argv) / sizeof(char*);
  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.log_level, 0);
}

static void one_extra_arg_is_destination (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "sip:alice@example.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_PING);
  assert_string_equal ("sip:alice@example.com", app->cfg.dest.ptr);
}

static void arg_cmd_ping_and_dest_set (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "ping", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_PING);
  assert_string_equal ("sip:bob@foo.com", app->cfg.dest.ptr);
}

static void arg_cmd_ping_and_dest_set_case_insens (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "PiNg", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_PING);
  assert_string_equal ("sip:bob@foo.com", app->cfg.dest.ptr);
}

static void arg_invalid_cmd (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "foo", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.cmd, CMD_UNKNOWN);
  assert_string_equal ("sip:bob@foo.com", app->cfg.dest.ptr);
}

static void enable_color_argument (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--color", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_COLOR, app->cfg.log_decor & PJ_LOG_HAS_COLOR);
}

static void print_trailing_dot (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--trail-dot", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_TRUE, app->cfg.trail_dot);
}

static void log_has_time (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--log-time" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_TIME, app->cfg.log_decor & PJ_LOG_HAS_TIME);
  assert_int_equal (PJ_LOG_HAS_MICRO_SEC, app->cfg.log_decor & PJ_LOG_HAS_MICRO_SEC);
}

static void log_has_level (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--log-level" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_LEVEL_TEXT, app->cfg.log_decor & PJ_LOG_HAS_LEVEL_TEXT);
}

static void log_has_sender (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--log-snd" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (PJ_LOG_HAS_SENDER, app->cfg.log_decor & PJ_LOG_HAS_SENDER);
}

static void set_local_port_long (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--local-port=8899" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (8899, app->cfg.local_port);
}

static void set_local_port_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-P 9988" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (9988, app->cfg.local_port);
}

static void set_username_long (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--username=myuser" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("myuser", app->cfg.username.ptr);
}

static void set_username_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-u bobuser" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("bobuser", app->cfg.username.ptr);
}

static void set_local_host_long (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--local-host=127.0.0.5" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("127.0.0.5", app->cfg.local_host.ptr);
}

static void set_local_host_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-l 127.0.0.8" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("127.0.0.8", app->cfg.local_host.ptr);
}

static void set_from_name_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-F \"Hello World!\"" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("\"Hello World!\"", app->cfg.from_name.ptr);
}

static void set_from_name_long (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--from-name=\"Gianni Schicchi\"" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("\"Gianni Schicchi\"", app->cfg.from_name.ptr);
}

static void set_content_type_xpidf_long (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--content-type=xpidf" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("application", app->cfg.ctype_media.type.ptr);
  assert_string_equal ("xpidf+xml", app->cfg.ctype_media.subtype.ptr);
  assert_int_equal (app->cfg.ctype_e, CTYPE_XPIDF);
}

static void set_content_type_pidf_short (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-C pidf" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("application", app->cfg.ctype_media.type.ptr);
  assert_string_equal ("pidf+xml", app->cfg.ctype_media.subtype.ptr);
  assert_int_equal (app->cfg.ctype_e, CTYPE_PIDF);
}

static void set_content_type_mwi (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-C mwi" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("application", app->cfg.ctype_media.type.ptr);
  assert_string_equal ("simple-message-summary", app->cfg.ctype_media.subtype.ptr);
  assert_int_equal (app->cfg.ctype_e, CTYPE_MWI);
}

/*
static void set_content_type_unknown_with_delim (void **state)
{
  (void) *state;
  pj_status_t status;
  struct sippak_app app;
  char *argv[] = { "./sippak", "-C text/plain" };
  int argc = sizeof(argv) / sizeof(char*);

  char *type = malloc(app.cfg.ctype_media.type.slen + 1);
  sippak_init(&app);
  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);

  strncpy(type, app.cfg.ctype_media.type.ptr, app.cfg.ctype_media.type.slen);
  type[app.cfg.ctype_media.type.slen] = '\0';
  assert_string_equal ("text", type);

  assert_string_equal ("plain", app.cfg.ctype_media.subtype.ptr);

  assert_int_equal (app.cfg.ctype_e, CTYPE_OTHER);
  free(type);
}
*/

static void set_content_type_unknown_without_delim (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "-C check-sync" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_string_equal ("application", app->cfg.ctype_media.type.ptr);
  assert_string_equal ("check-sync", app->cfg.ctype_media.subtype.ptr);
  assert_int_equal (app->cfg.ctype_e, CTYPE_OTHER);
}

static void set_mwi_list_init_zeros (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--mwi=0", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.mwi[0], 0); // new
  assert_int_equal (app->cfg.mwi[1], 0); // old
  assert_int_equal (app->cfg.mwi[2], 0); // urgent new
  assert_int_equal (app->cfg.mwi[3], 0); // urgent old
  assert_string_equal ("sip:bob@foo.com", app->cfg.mwi_acc.ptr);
}

static void set_mwi_list_with_acc (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--mwi=3,0,5", "--mwi-acc=sip:*97@example.com", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (app->cfg.mwi[0], 3); // new
  assert_int_equal (app->cfg.mwi[1], 0); // old
  assert_int_equal (app->cfg.mwi[2], 5); // urgent new
  assert_int_equal (app->cfg.mwi[3], 0); // urgent old
  assert_string_equal ("sip:*97@example.com", app->cfg.mwi_acc.ptr);
}

static void set_custom_header (void **state)
{
  pj_status_t status;
  struct sippak_app *app = *state;
  char *argv[] = { "./sippak", "--header=\"Subject: Ping Pong\"", "sip:bob@foo.com" };
  int argc = sizeof(argv) / sizeof(char*);

  status = sippak_getopts (argc, argv, app);
  assert_int_equal (status, PJ_SUCCESS);
  assert_int_equal (1, app->cfg.hdrs.cnt);
  assert_string_equal ("Subject", app->cfg.hdrs.h[0]->name.ptr);
  assert_string_equal ("Ping Pong", app->cfg.hdrs.h[0]->hvalue.ptr);
}

int main(int argc, const char *argv[])
{
  pj_status_t status;
  pj_caching_pool cp;

  pj_log_set_level(0); // do not print pj debug on init

  pj_init();

  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);

  status = pjsip_endpt_create(&cp.factory, "TEST_GETOPTS", &endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pool = pjsip_endpt_create_pool(endpt, PROJECT_NAME, POOL_INIT, POOL_INCR);

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(pass_no_arguments_prints_usage, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(pass_help_long_opt, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(pass_help_short_opt, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(pass_version_long_opt, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(pass_version_short_opt, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(set_nameservers_list, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_verbosity_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_verbosity_multy_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_verbosity_long_no_val, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_verbosity_long_with_val, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(suppress_verbosity_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(suppress_verbosity_long, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(one_extra_arg_is_destination, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(arg_cmd_ping_and_dest_set, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(arg_cmd_ping_and_dest_set_case_insens, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(arg_invalid_cmd, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(enable_color_argument, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(print_trailing_dot, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(log_has_time, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(log_has_level, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(log_has_sender, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(set_local_port_long, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_local_port_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_username_long, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_username_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_local_host_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_local_host_long, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_from_name_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_from_name_long, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(set_content_type_xpidf_long, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_content_type_pidf_short, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_content_type_mwi, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_content_type_unknown_without_delim, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(set_mwi_list_init_zeros, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(set_mwi_list_with_acc, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(set_custom_header, setup_app, teardown_app),
  };

  status = cmocka_run_group_tests_name("Agruments parsing", tests, NULL, NULL);

  pjsip_endpt_release_pool (endpt, pool);
  pjsip_endpt_destroy(endpt);

  return status;
}
