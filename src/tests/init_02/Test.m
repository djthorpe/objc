#include <stdio.h>
#include "Test.h"

@implementation Test

+(void)run:(id)str {
    printf("CALLED +[Test run:@%p]\n", str);
}

@end
