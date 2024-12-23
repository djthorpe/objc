#include <stdlib.h>
#include <stdio.h>
#include <objc/runtime.h>
#include "runtime.h"

// Allocate and initialize an object.
id objc_alloc_init(Class cls) {
    printf("objc_alloc_init\n");

    // TODO: Allocate 100 bytes
    id o = malloc(100);
    if (o == nil) {
        return nil;
    }

    // Set the class
    o->isa = cls;

    printf("  returns %p with class %p\n",o, o->isa);

    // Return the object
    return o;
}
