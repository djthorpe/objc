#include "Object+Description.h"

@implementation Object (Description)
+(NXConstantString* ) description {
    Class cls = [self class];
    return [NXConstantString withCString:class_getName(cls)];
}
@end
