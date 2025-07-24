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
      objc_assert([NXAutoreleasePool currentPool]);
      trueNumber = [[[NXNumberBool alloc] initWithBool:YES] autorelease];
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
      objc_assert([NXAutoreleasePool currentPool]);
      falseNumber = [[[NXNumberBool alloc] initWithBool:NO] autorelease];
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
 * @brief Get the stored value as a int64_t.
 */
- (int64_t)int64Value {
  return _value ? 1 : 0;
}

/**
 * @brief Get the stored value as a uint64_t.
 */
- (uint64_t)unsignedInt64Value {
  return _value ? 1 : 0;
}

/**
 * @brief Return the string representation of the boolean value.
 */
- (NXString *)description {
  return [NXString stringWithCString:(_value ? "true" : "false")];
}

/**
 * @brief Check for equality with another NXNumber instance.
 */
- (BOOL)isEqual:(id)object {
  if (![object isKindOfClass:[NXNumberBool class]]) {
    return NO;
  }
  NXNumberBool *other = (NXNumberBool *)object;
  return _value == other->_value;
}

@end
