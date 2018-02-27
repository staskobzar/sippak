#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "sippak.h"

static void foo (void **state)
{
  (void) *state;
  struct sippak_app app;
  assert_int_equal (1,1);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(foo)
  };
  return cmocka_run_group_tests_name("Agruments parsing", tests, NULL, NULL);
}
