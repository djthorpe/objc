#include "NXNumberBool.h"
#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instances of NXNumberBool for true, false
static NXNumberBool *trueNumber;
static NXNumberBool *falseNumber;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberBool

/**
 * @brief Initialize an instance with a boolean value.
 */
- (id)initWithBool:(BOOL)value {
  self = [super init];
  if (self) {
    _value = value;
  }
  return self;
}

/**
 * @brief Return a true instance
 */
+ (NXNumber *)trueValue {
  @synchronized(self) {
    if (trueNumber == nil) {
      trueNumber = [[NXNumberBool alloc] initWithBool:YES];
    }
  }
  return trueNumber;
}

/**
 * @brief Return a false instance
 */
+ (NXNumber *)falseValue {
  @synchronized(self) {
    if (falseNumber == nil) {
      falseNumber = [[NXNumberBool alloc] initWithBool:NO];
    }
  }
  return falseNumber;
}

/**
 * @brief Get the stored value as a boolean.
 */
- (BOOL)boolValue {
  return _value;
}

/**
 * @brief Return the string representation of the boolean value.
 */
- (NXString *)description {
  return [[[NXString alloc] initWithCString:(_value ? "true" : "false")]
      autorelease];
}

@end
