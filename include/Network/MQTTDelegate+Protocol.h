/**
 * @file MQTTDelegate+Protocol.h
 * @brief Defines the MQTTDelegate for MQTT events.
 *
 * This file defines the MQTTDelegate protocol which provides a
 * standardized interface for handling MQTT events in the Network
 * framework. Classes conforming to this protocol can implement methods for
 * handling these events.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for receiving MQTT events.
 * @headerfile MQTTDelegate+Protocol.h Network/Network.h
 * @ingroup Network
 *
 * Classes conforming to this protocol receive callbacks for MQTT events:
 * one invocation per message received. The delegate should return YES
 * if the message should be processed.
 */
@protocol MQTTDelegate

@end
