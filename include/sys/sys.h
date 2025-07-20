/**
 * @file sys.h
 * @brief Defines system management functions.
 *
 * This file declares various system methods for managing memory, threads
 * and resources.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Allocates a block of memory.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or `NULL` if the allocation fails.
 */
extern void *sys_malloc(size_t size);

/**
 * @brief Frees a block of memory.
 * @param ptr A pointer to the memory block to be deallocated.
 */
extern void sys_free(void *ptr);

/**
 * @brief Pauses the execution of the current thread for a specified time.
 * @param msec The number of milliseconds to sleep.
 */
extern void sys_sleep(int32_t msec);

/**
 * @brief Outputs a string to the system console or standard output.
 * @details This function writes the specified null-terminated string to the
 * system's standard output stream, and flushes the output buffer.
 * @param str A pointer to a null-terminated string to be output.
 */
extern void sys_puts(const char *str);

/**
 * @brief Outputs a character to the system console or standard output.
 * @details This function writes the specified character to the
 * system's standard output stream, but does not flush the output buffer.
 * @param ch The character to be output.
 */
extern void sys_putch(const char ch);

/**
 * @brief Returns a random number as a 32-bit unsigned integer.
 */
extern uint32_t sys_random_uint32(void);

/**
 * @brief Aborts the current process.
 */
extern void sys_abort(void);
