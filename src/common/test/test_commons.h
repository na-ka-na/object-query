/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <iostream>
#include "global_include.h"

#ifndef EXPECT_EQ
#define EXPECT_EQ(expected, actual, ...) \
  ASSERT(expected == actual, "expected:", expected, "actual:", actual, ##__VA_ARGS__)
#endif

#ifndef EXPECT_NE
#define EXPECT_NE(expected, actual, ...) \
  ASSERT(expected != actual, "expected:", expected, "actual:", actual, ##__VA_ARGS__)
#endif

#ifndef EXPECT_TRUE
#define EXPECT_TRUE(actual, ...) \
  ASSERT(static_cast<bool>(actual), "actual not true:", actual, ##__VA_ARGS__)
#endif

#ifndef EXPECT_FALSE
#define EXPECT_FALSE(actual, ...) \
  ASSERT(!static_cast<bool>(actual), "actual not false:", actual, ##__VA_ARGS__)
#endif
