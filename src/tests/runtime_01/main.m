#include <string.h>
#include <objc/objc.h>
#include <tests/tests.h>

int main() {
    Object* obj = [[Object alloc] init];
    test_assert(obj != NULL);

    const char *className = object_getClassName(obj);
    test_assert(className != NULL);
    test_assert(strcmp(className, "Object") == 0);

    Class cls = object_getClass(obj);
    test_assert(cls != NULL);

    const char *clsName = class_getName(cls);
    test_assert(clsName != NULL);
    test_assert(strcmp(clsName, "Object") == 0);

    // Check class
    test_assert(cls == [obj class]);
    test_assert([obj isEqual:obj] == YES);

    // Dispose of the object
    [obj dealloc];

    return 0;
}
