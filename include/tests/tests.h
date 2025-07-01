#pragma once

#define test_assert(condition) \
    if (!(condition)) { \
        panicf("Test failed: %s, file %s, line %d", #condition, __FILE__, __LINE__); \
    }

