
/**
 * @file Network.h
 * @brief Network framework header
 * @defgroup Network Network Framework
 * @ingroup objc
 *
 * Network-related classes and types, including WiFi adapter management,
 * connection management, data transmission and network protocols.
 */
#pragma once
#include <Foundation/Foundation.h>
#include <runtime-hw/hw.h>
#include <runtime-net/net.h>

#if __OBJC__

// Forward Declaration of classes
@class NXWireless;
@class NXWirelessNetwork;
@class NXNetworkTime;

// Import types
#import "NXWirelessError.h"

// Protocols and Category Definitions
#include "NetworkTimeDelegate+Protocol.h"
#include "WirelessDelegate+Protocol.h"

// Class Definitions
#include "NXNetworkTime.h"
#include "NXWireless.h"
#include "NXWirelessNetwork.h"

#endif // __OBJC__
