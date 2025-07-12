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
 * system's standard output stream. It provides a low-level interface for
 * text output that can be used by higher-level logging and display functions.
 * @param str A pointer to a null-terminated string to be output.
 */
extern void sys_puts(const char *str);
