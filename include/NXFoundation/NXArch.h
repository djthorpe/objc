/**
 * @file NXArch.h
 * @brief Architecture-related constants, types, and functions for NXFoundation.
 *
 * This header provides utilities for determining system architecture
 * properties such as bit width (32/64-bit) and byte order (endianness).
 * These functions are useful for writing portable code that adapts to
 * different hardware architectures.
 */
#pragma once

/**
 * @brief Enumeration representing different byte order (endianness) types.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
typedef enum {
  NXUnknownEndian = 0, /**< Unknown endianness */
  NXLittleEndian = 1,  /**< Least significant byte first */
  NXBigEndian = 2      /**< Most significant byte first */
} NXEndian;

/**
 * @brief Get the current architecture bits (32 or 64).
 * @return The architecture bits, either 32 or 64.
 *   Returns 0 if the architecture is unknown.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
uint8_t NXArchBits(void);

/**
 * @brief Get the current architecture endianness.
 * @return The architecture endianness, either NXLittleEndian or NXBigEndian.
 *   Returns 0 if the architecture is unknown.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
NXEndian NXArchEndian(void);

/**
 * @brief Get the number of CPU cores available on the current system.
 * @return The number of CPU cores available. Returns at least 1, even if
 *   the actual core count cannot be determined. Returns a maximum of 255 cores.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
uint8_t NXArchNumCores(void);
