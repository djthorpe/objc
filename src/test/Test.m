#include <stdio.h>
#include <stdbool.h>
#include <pico/stdlib.h>
#include "Test.h"

@implementation Test

+ (void)run {
    while (true) {
        printf("+[Test run]\n");
        sleep_ms(1000);
    }
}

@end
