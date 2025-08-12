#include "Test04.h"

@implementation Test04

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)initWithValue:(int)value {
  self->_value = value; // Set the instance variable
  return self;          // Return the initialized object
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

- (int)value {
  return self->_value; // Return the instance variable
}

- (void)setValue:(int)value {
  self->_value = value; // Update the instance variable
}

@end
