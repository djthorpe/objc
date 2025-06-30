#include <string.h>
#include <objc/objc.h>
#include "Test.h"

int main() {
    Test* test = [[Test alloc] init];
    assert(test != NULL);

    const char *className = object_getClassName(test);
    assert(className != NULL);
    assert(strcmp(className, "Test") == 0);

    Class cls = object_getClass(test);
    assert(cls != NULL);

    const char *clsName = class_getName(cls);
    assert(clsName != NULL);
    assert(strcmp(clsName, "Test") == 0);

    // Check class
    assert(cls == [test class]);
    assert([test isEqual:test] == YES);

    // Dispose of the object
    [test free];

    return 0;
}
