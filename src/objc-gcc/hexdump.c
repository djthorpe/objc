#include <stdio.h>
#include "hexdump.h"

#define isprint(c) ((c) >= 32 && (c) <= 126)

// hexdump function to print memory in hexadecimal format, with ascii representation
void hexdump(void* ptr, size_t size) {
    unsigned char* buf = (unsigned char*)ptr;
    size_t i, j;
    
    // Process the buffer 16 bytes at a time
    for (i = 0; i < size; i += 16) {
        // Print the address
        printf("%p  ", (void*)(buf + i));
        
        // Print hex values
        for (j = 0; j < 16; j++) {
            if (i + j < size)
                printf("%02x ", buf[i + j]);
            else
                printf("   "); // Padding for incomplete lines
            
            // Extra space after 8 bytes for readability
            if (j == 7)
                printf(" ");
        }
        
        // Print ASCII representation
        printf(" |");
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                // Print printable characters, or '.' for non-printable
                char c = buf[i + j];
                printf("%c", isprint(c) ? c : '.');
            } else {
                printf(" "); // Padding for incomplete lines
            }
        }
        printf("|\n");
    }
}
