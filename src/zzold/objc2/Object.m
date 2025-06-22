#include <stdio.h>

#include <objc/runtime.h>
#include <objc/Object.h>

@implementation Object

// Allocate memory for an instance of the class
+ (id)alloc
{
    printf("[Object alloc]\n");
    return 0;
}

- (id)init {
    printf("[Object init]\n");
    return self;
}

- (id)init2 {
    printf("[Object init2]\n");
    return self;
}

@end
