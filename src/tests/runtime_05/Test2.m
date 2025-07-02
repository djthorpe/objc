#include <stdio.h>
#include "Test2.h"

@implementation Test2

#pragma mark - Lifecycle
-(id) initWithValue:(int)value {
    self->_value2 = value; // Set the instance variable
    return self; // Return the initialized object
}

#pragma mark - Properties
-(int)value {
    return self->_value2; // Return the instance variable
}

-(void)setValue:(int)value {
    self->_value2 = value; // Update the instance variable
}

-(int) value2 {
    return self->_value2; // Return the instance variable
}

-(void) setValue2:(int)value {
    self->_value2 = value; // Update the instance variable
}

@end
