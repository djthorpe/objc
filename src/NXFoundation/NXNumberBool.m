#include "NXNumberBool.h"
#include <NXFoundation/NXFoundation.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instances of NXNumberBool for true, false
static NXNumberBool *trueNumber;
static NXNumberBool *falseNumber;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNumberBool

+ (void)initialize {
  // Do nothing - use lazy initialization instead
}

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
  if (trueNumber == nil) {
    trueNumber = [[NXNumberBool alloc] initWithBool:YES];
  }
  return trueNumber;
}

/**
 * @brief Return a false instance
 */
+ (NXNumber *)falseValue {
  if (falseNumber == nil) {
    falseNumber = [[NXNumberBool alloc] initWithBool:NO];
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
