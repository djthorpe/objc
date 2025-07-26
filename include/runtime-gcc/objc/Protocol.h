/**
 * @file Protocol.h
 * @brief Objective-C protocols definition.
 *
 * This file provides the definition of the Protocol class, which is used to
 * declare and implement Objective-C protocols.
 */
#pragma once

/**
 * @brief Protocol class definition
 * @headerfile Protocol.h objc/objc.h
 * @ingroup objc  
 */
@interface Protocol : Object {
@private
  const char *_name;
}

/**
 * @brief Returns the name of the protocol.
 * @return A C string containing the protocol name.
 */
- (const char *)name;

/**
 * @brief Checks if this protocol conforms to another protocol.
 * @param aProtocolObject The protocol to check conformance against.
 * @return YES if this protocol conforms to the specified protocol, NO
 * otherwise.
 *
 * This method determines whether this protocol adopts or inherits
 * from the specified protocol. A protocol conforms to another protocol if it
 * explicitly declares that it adopts the protocol, or if it inherits
 * from a protocol that conforms to the specified protocol.
 */
- (BOOL)conformsTo:(Protocol *)aProtocolObject;

@end
