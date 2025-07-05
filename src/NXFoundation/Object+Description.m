#include <NXFoundation/NXFoundation.h>

@implementation Object (Description)

- (NXString*) description {
    if (self == nil) {
        return @"<nil>";
    }
    return object_getClassName(self);
}

@end