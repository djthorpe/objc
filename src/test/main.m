#include <stdio.h>
#include <pico/stdlib.h>
#include <objc/objc.h>
#include "Test.h"

int main() {
    stdio_init_all();
    sleep_ms(1000);
    printf("\n\ntest\n");

    // Initialize the classes
    objc_init();

    // Run the test
    [Test run];

    printf("Ended test\n");
    while (true) {
        sleep_ms(1000);
        printf(".");
    }
    return 0;
}
