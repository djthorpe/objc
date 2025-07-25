#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Re-usable static instance of NXNull
static NXNull *nullValue;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

@implementation NXNull

+ (NXNull *)nullValue {
  @synchronized(self) {
    if (nullValue == nil) {
      objc_assert([NXAutoreleasePool currentPool]);
      nullValue = [[[NXNull alloc] init] autorelease];
    }
  }
  return nullValue;
}

/**
 * @brief Return the string representation of a null value.
 */
- (NXString *)description {
  return [NXString stringWithCString:"null"];
}

/**
 * @brief Return the JSON string representation of a null value.
 */
- (NXString *)JSONString {
  return [self description];
}

@end
