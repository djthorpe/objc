#include <NXFoundation/NXFoundation.h>
#include <string.h>

@implementation NXString

#pragma mark - Lifecycle

/**
 * @brief Initializes a new string by referencing another string.
 */
- (id)initWithString:(id)other {
  self = [super init];
  if (self == nil || self == other) {
    return nil;
  }
  _value = nil;
  _alloc = NO;
  if (other != nil) {
    if ([other class] == [NXConstantString class]) {
      _value = other;
    } else if ([other isKindOfClass:[NXString class]]) {
      _value = [other retain];
    } else {
      [self release];
      return nil;
    }
  }
  return self;
}

/**
 * @brief Initialize a new string by referencing a c-string.
 */
- (id)initWithCString:(const char *)cStr {
  self = [super init];
  if (self == nil) {
    return nil;
  }
  _value = [[NXConstantString alloc] initWithCString:cStr];
  _alloc = YES; // We need to de-allocate the constant string when de-allocating
                // this NXString
  return self;
}

/**
 * @brief Releases the string's internal value.
 */
- (void)dealloc {
  if (_value != nil) {
    if ([_value class] == [NXConstantString class] && _alloc) {
      [_value release]; // Release the constant string if it was allocated
    } else if ([_value isKindOfClass:[NXString class]]) {
      [_value release];
    }
  }
  [super dealloc];
}

#pragma mark - Methods

/**
 * @brief Returns the C-string representation of the string.
 */
- (const char *)cStr {
  if (_value != nil) {
    return [_value cStr];
  }
  return NULL;
}

/**
 * @brief Returns the length of the string.
 */
- (unsigned int)length {
  if (_value != nil) {
    return [_value length];
  }
  return 0;
}

@end
