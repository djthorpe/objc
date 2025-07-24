#include <pico/random.h>
#include <runtime-sys/sys.h>
#include <stdint.h>

/**
 * @brief Returns a random number as a 32-bit unsigned integer.
 */
uint32_t sys_random_uint32(void) { return get_rand_32(); }

/**
 * @brief Returns a random number as a 64-bit unsigned integer.
 */
uint64_t sys_random_uint64(void) { return get_rand_64(); }
