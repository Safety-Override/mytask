#include <gtest/gtest.h>
#include "../ServerFunctions.cpp"
#include "../ClientFunctions.cpp"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);

  
}