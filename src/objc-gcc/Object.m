#include <stdio.h>
#include <objc-gcc/Object.h>

@implementation Object

-(Class) class {
  return object_getClass(self);
}

-(BOOL) isEqual:(id)anObject {
  return self == anObject;
}

@end
