
#include <stdio.h>
#include "debug.h"

void hexdump(void* ptr,size_t size) {
    static char buf[5];
    unsigned char* p = (unsigned char*)ptr;
    for (int i = 0; i < size; i++, p++) {
        if(p[i] < ' ' || p[i] > '~') {
            sprintf(buf,"%02X ",p[i]);
            printf("%s",buf);
        } else {
            printf("'%c' ",p[i]);
        }
    }
}
