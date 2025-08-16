#include <Foundation/Foundation.h>
#include <runtime-sys/sys.h>

void *__zone_malloc(size_t size) {
  void *ptr = sys_malloc(size);
#ifdef DEBUG
  NXLog(@"  __zone_malloc: size=%zu => @%p", size, ptr);
#endif
  return ptr;
}

void __zone_free(void *ptr) {
#ifdef DEBUG
  NXLog(@"  __zone_free => @%p", ptr);
#endif
  sys_free(ptr);
}

void *objc_malloc(size_t size) {
  void *ptr = [(NXZone *)[NXZone defaultZone] allocWithSize:size];
#ifdef DEBUG
  NXLog(@"objc_malloc: size=%zu => @%p", size, ptr);
#endif
  return ptr;
}

void objc_free(void *ptr) {
#ifdef DEBUG
  NXLog(@"objc_free: @%p", ptr);
#endif
  [[NXZone defaultZone] free:ptr];
}
