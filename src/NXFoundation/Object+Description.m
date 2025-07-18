#include <NXFoundation/NXFoundation.h>

@implementation Object (Description)

- (NXString *)description {
  return [[[NXString alloc] initWithCString:object_getClassName(self)]
      autorelease];
}

+ (NXString *)description {
  return [[[NXString alloc] initWithCString:class_getName(self)] autorelease];
}

@end
