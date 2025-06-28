#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef PICO_PLATFORM
void sleep_ms(unsigned int ms);
#endif

// Define the panic function
void panicf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    puts("PANIC:");
    vprintf(fmt, args);
    va_end(args);
    puts("\n");
#ifdef PICO_PLATFORM
    while (true) {
       sleep_ms(1000);
    }
#else
    abort(); // Use abort to terminate the program
#endif
}
