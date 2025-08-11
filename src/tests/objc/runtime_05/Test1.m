#include "Test1.h"

@implementation Test1

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)initWithValue:(int)value {
  self->_value1 = value; // Set the instance variable
  return self;           // Return the initialized object
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

- (int)value {
  return self->_value1; // Return the instance variable
}

- (void)setValue:(int)value {
  self->_value1 = value; // Update the instance variable
}

- (int)value1 {
  return self->_value1; // Return the instance variable
}

- (void)setValue1:(int)value {
  self->_value1 = value; // Update the instance variable
}

@end
