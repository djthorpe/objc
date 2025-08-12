#include "TestClass.h"
#include <objc/objc.h>

@implementation Test

+ (void)run:(id)str {
  sys_printf("CALLED +[Test run:@%p]\n", str);
}

@end
