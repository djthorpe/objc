/**
 * @file NXArch.h
 * @brief Architecture-related constants, types, and functions for NXFoundation.
 * @details This header provides utilities for determining system architecture
 * properties such as bit width (32/64-bit) and byte order (endianness).
 * These utilities include functions to retrieve the current architecture's
 * bit width, endianness, and the number of CPU cores available. They are
 * designed to help developers write portable code that adapts to different
 * hardware architectures seamlessly.
 */
#pragma once

/**
 * @brief Enumeration representing different byte order (endianness) types.
 * @ingroup Foundation
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
 * @ingroup Foundation
 * @return The architecture bits, either 32 or 64.
 *   Returns 0 if the architecture is unknown.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
uint8_t NXArchBits(void);

/**
 * @brief Get the current operating system name.
 * @ingroup Foundation
 * @return A string containing the operating system name (e.g., "darwin",
 * "linux", "pico").
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
const char *NXArchOS(void);

/**
 * @brief Get the current processor/CPU type.
 * @ingroup Foundation
 * @return A string containing the processor name (e.g., "arm64", "x86-64",
 * "aarch64", "cortex-m0plus").
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
const char *NXArchProcessor(void);

/**
 * @brief Get the current architecture endianness.
 * @ingroup Foundation
 * @return The architecture endianness, either NXLittleEndian or NXBigEndian.
 *   Returns 0 if the architecture is unknown.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
NXEndian NXArchEndian(void);

/**
 * @brief Get the number of CPU cores available on the current system.
 * @ingroup Foundation
 * @return The number of CPU cores available. Returns at least 1, even if
 *   the actual core count cannot be determined. Returns a maximum of 255 cores.
 *
 * \headerfile NXArch.h NXFoundation/NXFoundation.h
 */
uint8_t NXArchNumCores(void);
