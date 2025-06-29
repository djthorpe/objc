#include <stdio.h>
#ifdef PICO_PLATFORM
#include <pico/stdlib.h>
#endif
#include <objc-gcc/objc.h>
#include "Test.h"

int main() {
#ifdef PICO_PLATFORM
    stdio_init_all();
    sleep_ms(1000);
#endif
    printf("\n\ntest\n");

    // Initialize the classes
    objc_init();

    // Run the test
    [Test run];

    printf("Ended test\n");
#ifdef PICO_PLATFORM
    while (true) {
        sleep_ms(1000);
        printf(".");
    }
#endif
    return 0;
}
