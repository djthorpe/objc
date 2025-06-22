#include <stdio.h>
#include <objc-gcc/Object.h>

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

@end
