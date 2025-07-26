#include <runtime-sys/sys.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Allocate memory
 */
void *sys_malloc(size_t size) { return malloc(size); }

/**
 * @brief Free allocated memory
 */
void sys_free(void *ptr) { return free(ptr); }

/**
 * @brief Set memory to a specific value
 */
void *sys_memset(void *ptr, uint8_t value, size_t size) {
  if (ptr == NULL)
    return NULL;
  if (size == 0)
    return ptr;
  memset(ptr, value, size);
  return ptr;
}

/**
 * @brief Copy memory from source to destination
 */
void *sys_memcpy(void *dest, const void *src, size_t size) {
  if (dest == NULL || src == NULL)
    return NULL;
  if (size == 0)
    return dest;
  memcpy(dest, src, size);
  return dest;
}

/**
 * @brief Move memory from source to destination (handles overlapping regions)
 */
void *sys_memmove(void *dest, const void *src, size_t size) {
  if (dest == NULL || src == NULL)
    return NULL;
  if (size == 0)
    return dest;
  memmove(dest, src, size);
  return dest;
}

/**
 * @brief Compare two memory blocks
 */
int sys_memcmp(const void *ptr1, const void *ptr2, size_t num) {
  if (ptr1 == NULL || ptr2 == NULL || num == 0) {
    return 0;
  }
  return memcmp(ptr1, ptr2, num);
}
