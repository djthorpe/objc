/**
 * @file Object+Protocol.h
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for objects.
 * @headerfile Object+Protocol.h objc/objc.h
 * @ingroup objc
 *
 * The ObjectProtocol defines the minimal interface that objects must
 * implement to provide basic object introspection functionality.
 */
@protocol ObjectProtocol
@required
/**
 * @brief Returns the name of the class.
 * @return A C-string containing the name of the class.
 */
+ (const char *)name;

/**
 * @brief Compares the receiver to another object for equality.
 * @param anObject The object to compare with the receiver.
 * @return YES if the objects are equal, otherwise NO.
 *
 * When comparing two objects, this method should be overridden to
 * compare the contents of the objects.
 */
- (BOOL)isEqual:(id)anObject;

/**
 * @brief Returns a Boolean value that indicates whether the receiver is an
 * instance of a given class.
 * @param cls A class object.
 * @return YES if the receiver is an instance of cls or an instance of any class
 * that inherits from cls, otherwise NO.
 */
- (BOOL)isKindOfClass:(Class)cls;

/**
 * @brief Checks if the receiver's class conforms to a protocol.
 * @param aProtocolObject The protocol to check conformance against.
 * @return YES if the receiver's class conforms to the specified protocol, NO
 * otherwise.
 */
- (BOOL)conformsTo:(Protocol *)aProtocolObject;

/**
 * @brief Checks if the receiver responds to a selector.
 * @param aSelector The selector to check for.
 * @return YES if the receiver responds to the specified selector, NO otherwise.
 */
- (BOOL)respondsToSelector:(SEL)aSelector;

@end
