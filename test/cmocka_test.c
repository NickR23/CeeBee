#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"

#include "ceebee/jumptable_test.h"

void test_success (void ** state) {
  assert_null(NULL);
}

int setup (void ** state) {
  return 0;
}

int teardown (void ** state) {
  return 0;
}

int main(void) {
  const struct CMUnitTest tests [] =
  {
    cmocka_unit_test(test_setCF),
    cmocka_unit_test(test_fail),
  };
  
  int count_fail_tests = cmocka_run_group_tests(tests, setup, teardown);
  
  return count_fail_tests;
}
