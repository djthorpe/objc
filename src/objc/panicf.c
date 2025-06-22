#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

void sleep_ms(unsigned int ms);

// Define the panic function
void panicf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    puts("PANIC:");
    vprintf(fmt, args);
    va_end(args);
    puts("\n");
    while (true) {
        sleep_ms(1000);
    }
}
