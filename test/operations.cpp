#include "gtest/gtest.h"

extern "C" {
  #include "ceebee/operations.h"
}

TEST(test_add, ok) {
  ASSERT_EQ(add(2,5), 7);
}

TEST(false_test_add, ok) {
  ASSERT_EQ(add(2,5), 8);
}
