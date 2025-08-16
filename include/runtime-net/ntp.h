/**
 * @file ntp.h
 * @brief Network Time Protocol (NTP) interface
 * @defgroup NTP Network Time Protocol
 * @ingroup Network
 *
 * Network Time Protocol (NTP) client to synchronize system time.
 *
 * This module retrieves the current UTC time from a time server, and
 * invokes a callback which can then update the system time.
 *
 * @example examples/runtime/ntp/main.c
 */
#pragma once
#include <runtime-sys/date.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Opaque NTP manager handle.
 * @ingroup NTP
 *
 * The handle identifies the singleton NTP manager. It is valid after
 * net_ntp_init() succeeds and remains valid until net_ntp_finalize() is
 * called.
 */
typedef struct net_ntp_t net_ntp_t;

/**
 * @brief Callback invoked when system time should be updated.
 * @ingroup NTP
 * @param date Pointer to the updated UTC time. Valid only for the duration of
 * the callback.
 */
typedef void (*net_ntp_callback_t)(const sys_date_t *date);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize (or re-initialize) the NTP manager.
 * @ingroup NTP
 * @param callback Optional callback invoked on successful time updates.
 * @return A pointer to the NTP manager handle (singleton), or NULL on error.
 */
net_ntp_t *net_ntp_init(net_ntp_callback_t callback);

/**
 * @brief Determine if the NTP handle is checking for time updates.
 * @ingroup NTP
 * @param ntp The NTP manager handle.
 * @return true if initialized; false otherwise.
 *
 * Returns true if the NTP manager is valid and connected. If the connection
 * is lost, it will return false.
 */
bool net_ntp_valid(net_ntp_t *ntp);

/**
 * @brief Release NTP resources
 * @ingroup NTP
 * @param ntp The NTP manager handle.
 */
void net_ntp_finalize(net_ntp_t *ntp);
