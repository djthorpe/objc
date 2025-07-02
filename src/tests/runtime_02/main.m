#include <string.h>
#include <objc/objc.h>
#include <tests/tests.h>
#include "Test.h"

int main() {
    Test* test = [[Test alloc] init];
    test_assert(test != NULL);

    const char *className = object_getClassName(test);
    test_assert(className != NULL);
    test_assert(strcmp(className, "Test") == 0);

    Class cls = object_getClass(test);
    test_assert(cls != NULL);

    const char *clsName = class_getName(cls);
    test_assert(clsName != NULL);
    test_assert(strcmp(clsName, "Test") == 0);

    // Check class
    test_assert(cls == [test class]);
    test_assert([test isEqual:test] == YES);

    // Dispose of the object
    [test dealloc];

    return 0;
}
