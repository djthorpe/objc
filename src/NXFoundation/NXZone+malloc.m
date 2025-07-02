#include <NXFoundation/NXFoundation.h>
#include <stdlib.h>

void* __zone_malloc(size_t size) {
    return malloc(size);
}

void __zone_free(void* ptr) {
    free(ptr);
}

void* objc_malloc(size_t size) {
    void* ptr = [[NXZone defaultZone] alloc:size];
#ifdef DEBUG
    NXLog(@"objc_malloc: size=%zu => @%p", size, ptr);
#endif
    return ptr;
}

void objc_free(void* ptr) {
#ifdef DEBUG
    NXLog(@"objc_free: @%p", ptr);
#endif
    [[NXZone defaultZone] free:ptr];
}
