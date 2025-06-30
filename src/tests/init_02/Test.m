#include <objc/objc.h>
#include "Test.h"

@implementation Test

+(void)run:(id)str {
    NSLog(@"CALLED +[Test run:@%p]\n", str);
}

@end
