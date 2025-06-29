#pragma once

#define assert(condition) \
    if (!(condition)) { \
        panicf("Assertion failed: %s, file %s, line %d", #condition, __FILE__, __LINE__); \
    }
