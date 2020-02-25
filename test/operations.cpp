#include "gtest/gtest.h"

extern "C" {
  #include "ceebee/operations.h"
}

TEST(operations, ok) {
  ASSERT_EQ(add(2,5), 7);
}
