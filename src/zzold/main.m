#include <stdio.h>
#ifdef PICO_RP2040
#include <pico/stdlib.h>
#endif
#include <objc-gcc/objc.h>
#include "Test.h"

int main() {
#ifdef PICO_RP2040
    stdio_init_all();
    sleep_ms(1000);
#endif
    printf("\n\ntest\n");

    // Run the test
    [Test run:@"Hello, World!"];

    printf("Ended test\n");
#ifdef PICO_RP2040
    while (true) {
        sleep_ms(1000);
        printf(".");
    }
#endif
    return 0;
}

