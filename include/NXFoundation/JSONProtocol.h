/**
 * @file JSONProtocol.h
 * @brief Defines a protocol that adds JSON methods for an Object.
 */
#pragma once

/**
 * @protocol JSONProtocol
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

@end
