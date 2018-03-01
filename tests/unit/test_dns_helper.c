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
  assert_string_equal ("2.2.3.4", ns[0].ptr);
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
  assert_string_equal ("2.2.3.4", ns[0].ptr);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(argument_ns_with_single_server_no_port),
    cmocka_unit_test(argument_ns_with_single_server_with_port),
  };
  return cmocka_run_group_tests_name("DNS helper", tests, NULL, NULL);
}
