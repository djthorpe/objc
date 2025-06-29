#include <stdio.h>
#include <string.h>
#include <objc-gcc/objc.h>

int main() {
    printf("\n\n=> object_01 test...\n\n");

    Object* obj = [[Object alloc] init];
    assert(obj != NULL);

    const char *className = object_getClassName(obj);
    assert(className != NULL);
    assert(strcmp(className, "Object") == 0);

    Class cls = object_getClass(obj);
    assert(cls != NULL);

    const char *clsName = class_getName(cls);
    assert(clsName != NULL);
    assert(strcmp(clsName, "Object") == 0);

    // Check class
    assert(cls == [obj class]);
    assert([obj isEqual:obj] == YES);

    // Dispose of the object
    [obj free];

    printf("\n\n<= object_01 test...\n\n");

    return 0;
}
