#include "Test+Description.h"

@implementation Test (Description)

-(NXConstantString* ) description {
    return self->_value;
}

@end
