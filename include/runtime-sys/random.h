/**
 * @file random.h
 * @brief Random number generation.
 * @defgroup SystemRandom Random Number Generation
 * @ingroup System
 *
 * System methods for generating random numbers, sometimes using hardware
 * to provide better entropy.
 */
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns a random number as a 32-bit unsigned integer.
 * @ingroup SystemRandom
 * @return A random unsigned 32-bit integer value.
 * @warning The function may not be thread-safe depending on the platform
 *          implementation.
 */
extern uint32_t sys_random_uint32(void);

/**
 * @brief Returns a random number as a 64-bit unsigned integer.
 * @ingroup SystemRandom
 * @return A random unsigned 64-bit integer value.
 * @warning The function may not be thread-safe depending on the platform
 *          implementation.
 */
extern uint64_t sys_random_uint64(void);

#ifdef __cplusplus
}
#endif
