#include <string.h>
#include <objc/objc.h>

@implementation NXConstantString

-(const char *) cStr {
    return self->data;
}

-(unsigned int) length {
    return self->length;
}

-(BOOL) isEqual:(id)anObject {
    if (self == anObject) {
        return YES;
    }
    if ([anObject class] == [self class]) {
        if (self->length != ((NXConstantString* )anObject)->length) {
            return NO;
        }
        return (memcmp(self->data, ((NXConstantString* )anObject)->data, self->length) == 0);
    }
    return NO;
}

@end
