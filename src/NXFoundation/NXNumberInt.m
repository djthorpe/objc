#include "NXNumberInt.h"
#include <NXFoundation/NXFoundation.h>

@implementation NXNumberInt

/**
 * @brief Return an instance of an integer value.
 */
+ (id)numberWithInt:(int)value {
  NXNumberInt *instance = [[self alloc] init];
  if (instance) {
    instance->_value = value; // Assuming _value is a private ivar of type int
  }
  return [instance autorelease];
}

/**
 * @brief Get the stored value as a signed integer.
 */
- (int)intValue {
  return _value;
}

/**
 * @brief Get the stored value as an unsigned integer.
 */
- (unsigned int)unsignedIntValue {
  return (unsigned int)_value;
}

@end
