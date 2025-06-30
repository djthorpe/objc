#include "Test.h"

@implementation Test

#pragma mark - Lifecycle
-(id) initWithString:(NXConstantString* )value {
    if (self = [super init]) { // Call the superclass initializer
        _value = value; // Set the instance variable
    }
    return self; // Return the initialized object
}

@end
