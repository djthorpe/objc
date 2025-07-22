#include "NXNumberBool.h"
#include <NXFoundation/NXFoundation.h>
#include <sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

int32_t NXRandInt32() { return (int32_t)sys_random_uint32(); }
uint32_t NXRandUnsignedInt32() { return sys_random_uint32(); }

///////////////////////////////////////////////////////////////////////////////
// NXNumber

@implementation NXNumber

+ (NXNumber *)numberWithBool:(BOOL)value {
  return value ? [NXNumberBool trueValue] : [NXNumberBool falseValue];
}

+ (NXNumber *)trueValue {
  return [NXNumberBool trueValue];
}

+ (NXNumber *)falseValue {
  return [NXNumberBool falseValue];
}

/**
 * @brief Returns the boolean value stored in this NXNumber instance.
 */
- (BOOL)boolValue {
  if ([self isKindOfClass:[NXNumberBool class]]) {
    return [(NXNumberBool *)self boolValue];
  }
  // If not a boolean number, return NO by default
  return NO;
}

@end
