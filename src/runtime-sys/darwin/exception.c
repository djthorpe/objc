#include <objc/objc.h>
#include <stddef.h>

int __gnu_objc_personality_v0(int version __attribute((unused)),
                              int actions __attribute((unused)),
                              uint64_t exceptionClass __attribute((unused)),
                              void *exceptionObject __attribute((unused)),
                              void *context __attribute((unused)),
                              BOOL isNew __attribute((unused))) {
  return 0;
}
