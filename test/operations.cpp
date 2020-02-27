#include "gtest/gtest.h"

extern "C" {
  #include "ceebee/operations.h"
  #include "ceebee/cpu.h"
}

TEST(test_add, ok) {
  ASSERT_EQ(add(2,5), 7);
}
