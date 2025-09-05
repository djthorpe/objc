/**
 * @file net.h
 * @brief Network interface abstraction layer
 * @defgroup RuntimeNet Network Interfaces
 * @ingroup System
 *
 * Managing network resources, protocols, and low-level operations.
 */
#pragma once
#include "mqtt.h"
#include "ntp.h"

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Occasional polling function for the network system.
 * @ingroup Network
 *
 * This function should be called periodically to allow the network
 * system to perform any necessary background tasks.
 */
void net_poll(void);
