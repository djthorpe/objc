#include <NXFoundation/NXFoundation.h>
#include <stdlib.h>

void* __zone_malloc(size_t size) {
    return malloc(size);
}

void __zone_free(void* ptr) {
    free(ptr);
}

void* objc_malloc(size_t size) {
    return [[NXZone defaultZone] alloc:size];
}

void objc_free(void* ptr) {
    [[NXZone defaultZone] free:ptr];
}
