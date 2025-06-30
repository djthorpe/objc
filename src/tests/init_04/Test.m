#include <stdio.h>
#include "Test.h"

@implementation Test

#pragma mark - Lifecycle
-(id) initWithValue:(int)value {
    self->value = value; // Set the instance variable
    return self; // Return the initialized object
}

#pragma mark - Properties
-(int)value {
    return value; // Return the instance variable
}

-(void)setValue:(int)value {
    self->value = value; // Set the instance variable
}

@end
