/**
 * @file MQTT.h
 * @brief MQTT client.
 *
 * MQTT client
 */
#pragma once

/**
 * @brief An MQTT client
 * @ingroup Network
 * @headerfile MQTT.h Network/Network.h
 */
@interface MQTT : NXObject {
@private
  NXString *_host;             ///< The MQTT host
  uint16_t _port;              ///< The MQTT port
  NXString *_clientIdentifier; ///< The MQTT client identifier
  net_mqtt_t *_mqtt;
}

/**
 * @brief Creates a new MQTT client.
 * @return A pointer to the newly created MQTT client, or NULL on failure.
 *
 * This method creates a new MQTT client instance, without connecting to the
 * host.
 */
+ (MQTT *)clientWithHost:(id<NXConstantStringProtocol>)host;

/**
 * @brief Creates a new MQTT client.
 * @return A pointer to the newly created MQTT client, or NULL on failure.
 *
 * This method creates a new MQTT client instance, without connecting to the
 * host, but also allows port, client identifier and timeout to be specified.
 */
+ (MQTT *)clientWithHost:(id<NXConstantStringProtocol>)host
                    port:(uint16_t)port
        clientIdentifier:(id<NXConstantStringProtocol>)clientIdentifier
                 timeout:(NXTimeInterval)timeout;

/**
 * @brief Gets the current MQTT delegate.
 * @return The current MQTT delegate, or nil if no delegate is set.
 *
 * This method returns the object that serves as the MQTT delegate.
 */
- (id<MQTTDelegate>)delegate;

/**
 * @brief Sets the MQTT delegate.
 * @param delegate The object to set as the MQTT delegate, or nil to
 * remove the current delegate.
 *
 * The delegate object will receive MQTT-related callbacks when the
 * MQTT state changes. The delegate should conform to the
 * MQTTDelegate protocol.
 */
- (void)setDelegate:(id<MQTTDelegate>)delegate;

/**
 * @brief Begin an asynchronous connection to the configured MQTT broker.
 * @return YES if the connection request was initiated; NO on immediate failure.
 *
 * Uses the host, port, and client identifier configured for this instance.
 * Connection progress and errors are delivered via the delegate callbacks.
 * This method does not block.
 */
- (BOOL)connect;

/**
 * @brief Begin an asynchronous connection with credentials.
 * @param user     Username for authentication.
 * @param password Password for authentication.
 * @return YES if the connection request was initiated; NO on immediate failure.
 *
 * This method behaves like @c -connect but supplies credentials to the broker.
 * Progress and errors are reported via the delegate. This method does not
 * block.
 */
- (BOOL)connectWithUser:(id<NXConstantStringProtocol>)user
               password:(id<NXConstantStringProtocol>)password;

/**
 * @brief Begin an asynchronous disconnect from the broker.
 * @return YES if the disconnect was requested; NO if not connected or request
 *         could not be queued.
 *
 * Completion is reported via the delegate. This method does not block.
 */
- (BOOL)disconnect;

/**
 * @brief Publish a string payload to a topic.
 * @param message String to publish
 * @param topic   Topic name to publish to; must be non-empty.
 * @param qos     Quality of Service level: 0, 1, or 2.
 * @param retain  When YES, sets the retain flag on the message.
 * @return YES if the message was queued; NO on immediate failure (not
 *         connected, invalid parameters, or resource exhaustion).
 *
 * The call is asynchronous and does not wait for broker acknowledgements.
 */
- (BOOL)publishString:(id<NXConstantStringProtocol>)message
              toTopic:(id<NXConstantStringProtocol>)topic
                  qos:(uint8_t)qos
               retain:(BOOL)retain;

/**
 * @brief Publish binary data to a topic.
 * @param data   Data payload to publish. May be empty.
 * @param topic  Topic name to publish to; must be non-empty.
 * @param qos    Quality of Service level: 0, 1, or 2.
 * @param retain When YES, sets the retain flag on the message.
 * @return YES if the message was queued; NO on immediate failure.
 *
 * Payload size may be limited by the underlying stack (typically ≤ 65535
 * bytes).
 */
- (BOOL)publishData:(NSData *)data
            toTopic:(id<NXConstantStringProtocol>)topic
                qos:(uint8_t)qos
             retain:(BOOL)retain;

/**
 * @brief Publish a JSON-encoded object to a topic.
 * @param json   Object conforming to JSONProtocol; serialized to JSON.
 * @param topic  Topic name to publish to; must be non-empty.
 * @param qos    Quality of Service level: 0, 1, or 2.
 * @param retain When YES, sets the retain flag on the message.
 * @return YES if the message was queued; NO on immediate failure or if JSON
 *         serialization fails.
 */
- (BOOL)publishJSON:(id<JSONProtocol>)json
            toTopic:(id<NXConstantStringProtocol>)topic
                qos:(uint8_t)qos
             retain:(BOOL)retain;

@end
