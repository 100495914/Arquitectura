//
// Created by diego on 15/11/24.
//
#include <gtest/gtest.h>

// Para ejecutar todos los test
GTEST_API_ int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}