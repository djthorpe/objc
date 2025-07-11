#include <sys/sys.h>
#include <stdlib.h>

/*
 ** Allocate memory
 */
void* sys_malloc(size_t size) {
    return malloc(size);
}

/*
 ** Free allocated memory
 */
void sys_free(void* ptr) {
    return free(ptr);
}
