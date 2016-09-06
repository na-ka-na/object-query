#pragma once

#include <iostream>
#include "generated_common.h"

#ifndef EXPECT_EQ
#define EXPECT_EQ(expected, actual, ...) \
  ASSERT(expected == actual, "expected:", expected, "actual:", actual, ##__VA_ARGS__)
#endif

#ifndef EXPECT_NE
#define EXPECT_NE(expected, actual, ...) \
  ASSERT(expected != actual, "expected:", expected, "actual:", actual, ##__VA_ARGS__)
#endif
