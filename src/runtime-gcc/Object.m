#include <stdlib.h>
#include <objc/objc.h>

@implementation Object

+(id) alloc {
  id obj = (id)malloc(class_getInstanceSize(self));
  if (obj) {
    object_setClass(obj, self);
  }
  return obj;
}

-(id) init {
  return self;
}

-(void) dealloc {
  free(self);
}

-(Class) class {
  return object_getClass(self);
}

-(BOOL) isEqual:(id)anObject {
  return self == anObject;
}

@end
