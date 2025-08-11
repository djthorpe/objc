#include "Test.h"

@implementation Test

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

- (id)initWithString:(NXConstantString *)value {
  self = [super init];
  if (self != nil) {
    _value = value;
  }
  return self;
}

@end
