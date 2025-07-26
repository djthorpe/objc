/**
 * @file NXConstantString+Protocol.h
 * @brief Defines the NXConstantStringProtocol for constant string
 * functionality.
 *
 * This file defines the NXConstantStringProtocol which provides a standardized
 * interface for constant string objects in the NXFoundation framework. Classes
 * conforming to this protocol must implement basic string access methods.
 *
 * The protocol is designed to enable polymorphic behavior among different
 * constant string classes, allowing them to be used interchangeably in contexts
 * where basic string operations are needed.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for constant string objects.
 * @headerfile NXConstantString+Protocol.h objc/objc.h
 * @ingroup objc 
 *
 * The NXConstantStringProtocol defines the minimal interface that constant
 * string objects must implement to provide basic string functionality. This
 * enables different string implementations to be used polymorphically
 * throughout the framework.
 *
 * Classes conforming to this protocol can be used wherever basic string
 * operations such as accessing the C string representation or getting the
 * length are required.
 */
@protocol NXConstantStringProtocol

@required

/**
 * @brief Returns the C string representation of the string.
 * @return A pointer to a null-terminated C string containing the string's
 *         content. The returned pointer should not be modified or freed
 *         by the caller.
 */
- (const char *)cStr;

/**
 * @brief Returns the length of the string in bytes.
 * @return The length of the string in bytes, not including
 *         the null terminator.
 */
- (unsigned int)length;

@end
