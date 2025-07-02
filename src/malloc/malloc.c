#include <objc/objc.h>
#include <stdlib.h>

/*
 ** Allocate memory
 */
void* objc_malloc(size_t size) {
    return malloc(size);
}

/*
 ** Free allocated memory
 */
void objc_free(void* ptr) {
    return free(ptr);
}
