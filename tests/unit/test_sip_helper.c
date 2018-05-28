#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

pjsip_endpoint *endpt;
pj_pool_t *pool;

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

static void create_from_header (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:alice@example.com");

  pj_str_t from = sippak_create_from_hdr(app);
  assert_string_equal("sip:alice@example.com", from.ptr);
}

static void create_from_header_username (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:alice@sipspot.com");
  app->cfg.username = pj_str("charlie");

  pj_str_t from = sippak_create_from_hdr(app);
  assert_string_equal("sip:charlie@sipspot.com", from.ptr);
}

static void create_from_header_display_name (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:bob@sip.com");
  app->cfg.username = pj_str("charlie");
  app->cfg.from_name = pj_str("Charlie & Co.");

  pj_str_t from = sippak_create_from_hdr(app);
  assert_string_equal("\"Charlie & Co.\" <sip:charlie@sip.com>", from.ptr);
}

static void create_ruri (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:harry@sip.com");

  pj_str_t ruri = sippak_create_ruri(app);
  assert_string_equal("sip:harry@sip.com", ruri.ptr);
}

static void create_reg_ruri (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:alice@sipproxy.com");
  pj_str_t ruri = sippak_create_reg_ruri(app);
  assert_string_equal("sip:sipproxy.com", ruri.ptr);
}

static void create_reg_ruri_tcp (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:john@sipproxy.com");
  app->cfg.proto = PJSIP_TRANSPORT_TCP;
  pj_str_t ruri = sippak_create_reg_ruri(app);
  assert_string_equal("sip:sipproxy.com;transport=tcp", ruri.ptr);
}

static void create_ruri_tcp (void **state)
{
  struct sippak_app *app = *state;

  app->cfg.dest = pj_str("sip:jane@sip.com");
  app->cfg.proto = PJSIP_TRANSPORT_TCP;

  pj_str_t ruri = sippak_create_ruri(app);
  assert_string_equal("sip:jane@sip.com;transport=tcp", ruri.ptr);
}

static void create_contact_hdr (void **state)
{
  struct sippak_app *app = *state;
  pj_str_t addr = pj_str("192.168.1.111");

  pj_str_t cnt = sippak_create_contact_hdr(app, &addr, 14255);
  assert_string_equal("sip:alice@192.168.1.111:14255", cnt.ptr);
}

static void create_contact_hdr_user (void **state)
{
  struct sippak_app *app = *state;
  pj_str_t addr = pj_str("192.168.18.11");

  app->cfg.username = pj_str("jonny");
  pj_str_t cnt = sippak_create_contact_hdr(app, &addr, 1255);
  assert_string_equal("sip:jonny@192.168.18.11:1255", cnt.ptr);
}

int main(int argc, const char *argv[])
{
  pj_status_t status;
  pj_caching_pool cp;

  pj_log_set_level(0); // do not print pj debug on init

  pj_init();

  pj_caching_pool_init(&cp, &pj_pool_factory_default_policy, 0);

  status = pjsip_endpt_create(&cp.factory, "TEST_SIP_HELPER", &endpt);
  PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

  pool = pjsip_endpt_create_pool(endpt, PROJECT_NAME, POOL_INIT, POOL_INCR);

  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(create_from_header, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(create_from_header_username, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(create_from_header_display_name, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(create_ruri, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(create_ruri_tcp, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(create_reg_ruri, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(create_reg_ruri_tcp, setup_app, teardown_app),

    cmocka_unit_test_setup_teardown(create_contact_hdr, setup_app, teardown_app),
    cmocka_unit_test_setup_teardown(create_contact_hdr_user, setup_app, teardown_app),
  };
  status = cmocka_run_group_tests_name("SIP packet helper", tests, NULL, NULL);

  pjsip_endpt_release_pool (endpt, pool);
  pjsip_endpt_destroy(endpt);

  return status;
}
