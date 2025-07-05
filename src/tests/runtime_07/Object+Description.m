#include "Object+Description.h"

@implementation Object (Description)
+(NXConstantString* ) description {
    Class cls = [self class];
    return [[NXConstantString alloc] initWithCString:class_getName(cls)];
}
@end
