#include <stdio.h>
#include <objc/objc.h>
#include <tests/tests.h>

/* Tests root class and a subclass with an ivar and accessor methods */

OBJC_ROOT_CLASS
@interface RootClass
{
  Class isa;
}

+(Class) class;
+(Class) superclass;

@end

@implementation RootClass
+(Class) class
{
  return self;
}

+(Class) superclass 
{
  return class_getSuperclass(self);
}

@end

@interface SubClass : RootClass
{
  int _state;
}
-(void) setState: (int)number;
-(int) state;
@end

@implementation SubClass
-(void) setState: (int)number {
  _state = number;
}

-(int) state {
  return _state;
}
@end

int main (void) {
  test_class_has_superclass("SubClass", "RootClass");
  test_class_has_instance_method("SubClass", @selector(setState:));
  test_class_has_instance_method("SubClass", @selector(state));
  return 0;
}
