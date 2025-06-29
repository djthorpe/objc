#include <stdio.h>
#include <stdbool.h>

#ifdef PICO_PLATFORM
#include <pico/stdlib.h>
#endif

#include "Test.h"

@implementation Test

+(void)run:(id)str {
    printf("CALLED +[Test run]\n");
}

@end
