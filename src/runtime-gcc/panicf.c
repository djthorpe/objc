#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// Define the panic function
void panicf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    puts("PANIC:");
    vprintf(fmt, args);
    va_end(args);
    puts("\n");
    abort(); // Use abort to terminate the program
}
