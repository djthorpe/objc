#include "NXNumberBool.h"
#include "NXNumberInt64.h"
#include "NXNumberUnsignedInt64.h"
#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

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

+ (NXNumber *)numberWithInt64:(int64_t)value {
  return [NXNumberInt64 numberWithInt64:value];
}

+ (NXNumber *)numberWithUnsignedInt64:(uint64_t)value {
  return [NXNumberUnsignedInt64 numberWithUnsignedInt64:value];
}

+ (NXNumber *)trueValue {
  return [NXNumberBool trueValue];
}

+ (NXNumber *)falseValue {
  return [NXNumberBool falseValue];
}

@end
