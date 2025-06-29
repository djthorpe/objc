#include <stdio.h>
#include <stdlib.h>
#include <objc-gcc/objc.h>

@implementation Object

+(id) alloc {
  id obj = (id)malloc(class_getInstanceSize(self));
  printf("+[%s alloc] size=%zu => @%p\n",class_getName(self), class_getInstanceSize(self), obj);
  if (obj) {
    object_setClass(obj, self);
  }
  return obj;
}

-(void) free {
  printf("-[%s free] @%p\n", object_getClassName(self), self);
  free(self);
}

-(Class) class {
  return object_getClass(self);
}

-(BOOL) isEqual:(id)anObject {
  return self == anObject;
}

@end
