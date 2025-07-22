/**
 * @file NXString+Protocol.h
 * @brief Defines the NXStringProtocol for string manipulation functionality.
 *
 * This file defines the NXStringProtocol which provides a standardized
 * interface for string-like objects in the NXFoundation framework. Classes
 * conforming to this protocol must implement basic string access methods.
 *
 * The protocol is designed to enable polymorphic behavior among different
 * string-like classes, allowing them to be used interchangeably in contexts
 * where basic string operations are needed.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for string-like objects.
 *
 * The NXStringProtocol defines the minimal interface that string-like objects
 * must implement to provide basic string functionality. This enables different
 * string implementations to be used polymorphically throughout the framework.
 *
 * Classes conforming to this protocol can be used wherever basic string
 * operations such as accessing the C string representation or getting the
 * length are required.
 *
 * \headerfile NXString+Protocol.h NXFoundation/NXFoundation.h
 */
@protocol NXStringProtocol

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
