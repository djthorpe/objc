#include <stdlib.h>
#include <objc/objc.h>

@implementation Object

+(id) alloc {
  id obj = (id)objc_malloc(class_getInstanceSize(self));
  if (obj) {
    object_setClass(obj, self);
  }
  return obj;
}

-(id) init {
  return self;
}

-(void) dealloc {
  objc_free(self);
}

-(Class) class {
  return object_getClass(self);
}

+(Class) class {
  return self;
}

-(Class) superclass {
  return object_getSuperclass(self);
}

+(Class) superclass {
  return class_getSuperclass(self);
}

-(const char* )name
{
  return object_getClassName(self);
}


+(const char* )name
{
  return class_getName(self);
}


-(BOOL) isEqual:(id)anObject {
  return self == anObject;
}

@end
