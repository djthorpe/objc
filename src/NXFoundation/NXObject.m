#include <NXFoundation/NXFoundation.h>

@implementation NXObject

-(id) init {
    NXLog(@"NXObject init called");
    self = [super init];
    if (!self) {
        [self dealloc];
    }
    return self;
}

@end
