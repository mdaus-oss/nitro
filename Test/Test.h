#pragma once

#include "gtest/gtest.h"

// use GTest macros to mimic existing unit-tests

#define TEST_ASSERT(X) EXPECT_TRUE(X)
#define TEST_ASSERT_NULL(X) TEST_ASSERT((X) == NULL)

#define TEST_ASSERT_EQ(X1, X2) EXPECT_EQ(X1, X2)
#define TEST_ASSERT_EQ_INT(X1, X2) TEST_ASSERT_EQ(X1, X2)
#define TEST_ASSERT_EQ_STR(X1, X2) EXPECT_STREQ(X1, X2)
#define TEST_ASSERT_EQ_FLOAT(X1, X2) EXPECT_FLOAT_EQ(X1, X2)

#define TEST_EXCEPTION(X) EXPECT_ANY_THROW(X)


#define TEST_MAIN(X)
