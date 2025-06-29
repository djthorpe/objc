#include <objc/objc.h>

@implementation NXConstantString

+(void) test {
}

-(const char *) cStr {
    return self->data;
}

-(unsigned int) length {
    return self->length;
}

@end
