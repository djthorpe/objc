#include <stdlib.h>
#include <sys/sys.h>

/*
 ** Allocate memory
 */
void *sys_malloc(size_t size) { return malloc(size); }

/*
 ** Free allocated memory
 */
void sys_free(void *ptr) { return free(ptr); }
