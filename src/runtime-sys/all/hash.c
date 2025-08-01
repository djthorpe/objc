#include <runtime-sys/hash.h>
#include <runtime-sys/sys.h>

/**
 * @brief djb2 hash function for strings
 */
uintptr_t sys_hash_djb2(const char *str) {
  sys_assert(str != NULL);

  uintptr_t hash = 5381; // Standard djb2 initial value
  uintptr_t c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash;
}
