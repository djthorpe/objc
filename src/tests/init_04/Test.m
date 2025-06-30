#include <stdio.h>
#include "Test.h"

@implementation Test

#pragma mark - Lifecycle
-(id) initWithValue:(int)value {
    self->_value = value; // Set the instance variable
    return self; // Return the initialized object
}

#pragma mark - Properties
-(int)value {
    return self->_value; // Return the instance variable
}

-(void)setValue:(int)value {
    //@synchronized(self) { 
        self->_value = value; // Update the instance variable
    //}
}

@end
