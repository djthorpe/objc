/**
 * @file JSONProtocol.h
 * @brief Defines a protocol that adds JSON methods for an Object.
 */
#pragma once

/**
 * @protocol JSONProtocol
 * @ingroup Foundation
 * @brief A protocol that defines JSON methods for an Object.
 *
 * \headerfile Object+JSON.h NXFoundation/NXFoundation.h
 */
@protocol JSONProtocol

@required
/**
 * @brief Returns a JSON representation of the instance.
 * @return A JSON string representation of the receiver.
 */
- (NXString *)JSONString;

/**
 * @brief Returns the appropriate capacity for the JSON
 * representation of the instance.
 * @return An approximate number of bytes required to represent the instance
 * in JSON format.
 *
 * This method is used to determine the approximate capacity required for the
 * JSON representation, which can be useful for memory allocation or performance
 * optimizations.
 */
- (size_t)JSONBytes;

@end
