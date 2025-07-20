#include "NXNumberUnsignedInt.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instances of NXNumberUnsignedInt for true, false
static NXNumberUnsignedInt *trueNumber;
static NXNumberUnsignedInt *falseNumber;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberUnsignedInt

+ (void)initialize {
  // Initialize the static instances of NXNumberUnsignedInt
  trueNumber = [[NXNumberUnsignedInt numberWithBool:YES] retain];
  objc_assert(trueNumber);
  falseNumber = [[NXNumberUnsignedInt numberWithBool:NO] retain];
  objc_assert(falseNumber);
}

/**
 * @brief Return an instance of an unsigned integer value.
 */
+ (id)numberWithUnsignedInt:(unsigned int)value {
  NXNumberUnsignedInt *instance = [[self alloc] init];
  if (instance) {
    instance->_value =
        value; // Assuming _value is a private ivar of type unsigned int
  }
  return [instance autorelease];
}

/**
 * @brief Return an instance of a boolean value.
 */
+ (id)numberWithBool:(BOOL)value {
  NXNumberUnsignedInt *instance = [[self alloc] init];
  if (instance) {
    instance->_value = value; // Store as unsigned int
  }
  return [instance autorelease];
}

/**
 * @brief Get the stored value as an unsigned integer.
 */
- (unsigned int)unsignedIntValue {
  return _value;
}

/**
 * @brief Get the stored value as a boolean.
 */
- (BOOL)boolValue {
  return _value ? YES : NO; // Non-zero values are considered YES
}

@end
