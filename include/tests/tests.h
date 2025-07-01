#pragma once

#define test_assert(condition) \
    if (!(condition)) { \
        panicf("Test failed: %s, file %s, line %d", #condition, __FILE__, __LINE__); \
    }

#define test_class_has_superclass(cls, supercls) \
    do { \
        test_assert((cls) != NULL && (supercls) != NULL); \
        Class _test_cls = objc_lookupClass(cls); \
        test_assert(_test_cls != Nil); \
        Class _test_supercls = objc_lookupClass(supercls); \
        Class _test_actual_super = class_getSuperclass(_test_cls); \
        test_assert(_test_actual_super == _test_supercls); \
    } while(0)

