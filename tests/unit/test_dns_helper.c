#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

static void argument_ns_with_single_server_no_port (void **state)
{
  (void) *state;
  pj_str_t ns[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  int servers_num = 0;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=2.2.3.4" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  servers_num = sippak_get_ns_list (&app, ns, ports);
  assert_int_equal (1, servers_num);
  assert_int_equal (53, ports[0]);
  assert_int_equal(0, pj_strcmp2(&ns[0], "2.2.3.4"));
}

static void argument_ns_with_single_server_with_port (void **state)
{
  (void) *state;
  pj_str_t ns[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  int servers_num = 0;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=2.2.3.4:1553" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  servers_num = sippak_get_ns_list (&app, ns, ports);
  assert_int_equal (1, servers_num);
  assert_int_equal (1553, ports[0]);
  assert_int_equal(0, pj_strcmp2(&ns[0], "2.2.3.4"));
}

static void argument_ns_with_multi_servers_no_port (void **state)
{
  (void) *state;
  pj_str_t ns[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  int servers_num = 0;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=2.2.3.4,127.0.0.1" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  servers_num = sippak_get_ns_list (&app, ns, ports);
  assert_int_equal (2, servers_num);
  assert_int_equal (53, ports[0]);
  assert_int_equal (53, ports[1]);
  assert_int_equal(0, pj_strcmp2(&ns[0], "2.2.3.4"));
  assert_int_equal(0, pj_strcmp2(&ns[1], "127.0.0.1"));
}

static void argument_ns_with_multi_servers_with_ports (void **state)
{
  (void) *state;
  pj_str_t ns[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  int servers_num = 0;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=2.2.3.4:553,22.10.10.8,199.8.8.50:89" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  servers_num = sippak_get_ns_list (&app, ns, ports);
  assert_int_equal (3, servers_num);
  assert_int_equal (553, ports[0]);
  assert_int_equal (53, ports[1]);
  assert_int_equal (89, ports[2]);
  assert_int_equal(0, pj_strcmp2(&ns[0], "2.2.3.4"));
  assert_int_equal(0, pj_strcmp2(&ns[1], "22.10.10.8"));
  assert_int_equal(0, pj_strcmp2(&ns[2], "199.8.8.50"));
}

static void argument_ns_respect_servers_num_limit (void **state)
{
  (void) *state;
  pj_str_t ns[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  int servers_num = 0;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=2.2.3.4,2.10.10.8,19.8.8.5,127.0.0.1,127.0.0.2,111.0.0.1" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  servers_num = sippak_get_ns_list (&app, ns, ports);
  assert_int_equal (3, servers_num);
  assert_int_equal(0, pj_strcmp2(&ns[0], "2.2.3.4"));
  assert_int_equal(0, pj_strcmp2(&ns[1], "2.10.10.8"));
  assert_int_equal(0, pj_strcmp2(&ns[2], "19.8.8.5"));
}

static void argument_ns_server_with_invalid_port (void **state)
{
  (void) *state;
  pj_str_t ns[MAX_NS_COUNT];
  pj_uint16_t ports[MAX_NS_COUNT];
  int servers_num = 0;
  struct sippak_app app;
  char *argv[] = { "./sippak", "--ns=2.2.3.4:abcd" };
  int argc = sizeof(argv) / sizeof(char*);
  sippak_init(&app);
  sippak_getopts (argc, argv, &app);
  servers_num = sippak_get_ns_list (&app, ns, ports);
  assert_int_equal (1, servers_num);
  assert_int_equal (53, ports[0]);
  assert_int_equal(0, pj_strcmp2(&ns[0], "2.2.3.4"));
}

int main(int argc, const char *argv[])
{
  pj_log_set_level(0); // do not print pj debug on init
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(argument_ns_with_single_server_no_port),
    cmocka_unit_test(argument_ns_with_single_server_with_port),
    cmocka_unit_test(argument_ns_with_multi_servers_no_port),
    cmocka_unit_test(argument_ns_with_multi_servers_with_ports),
    cmocka_unit_test(argument_ns_respect_servers_num_limit),
    cmocka_unit_test(argument_ns_server_with_invalid_port),
  };
  return cmocka_run_group_tests_name("DNS helper", tests, NULL, NULL);
}
