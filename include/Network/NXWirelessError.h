/**
 * @file NXWirelessError.h
 * @brief Defines the NXWirelessError enumeration for wireless connection
 * errors.
 *
 * This file provides the definition for NXWirelessError, which is used
 * to represent various error conditions that can occur during wireless
 * connection operations.
 */
#pragma once

/**
 * @brief Wireless connection error codes.
 * @ingroup Network
 */
typedef enum {
  NXWirelessErrorBadAuth =
      (1 << 0), ///< Authentication failed (bad credentials)
  NXWirelessErrorNotFound = (1 << 1), ///< Target network not found
  NXWirelessErrorGeneral = (1 << 2),  ///< General or unspecified error
} NXWirelessError;
