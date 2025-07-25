/**
 * @file Collection+Protocol.h
 * @brief Defines a protocol that adds collection methods for an Object.
 */
#pragma once

/**
 * @protocol CollectionProtocol
 * @brief A protocol that defines collection methods for an Object.
 *
 * \headerfile Collection+Collection.h NXFoundation/NXFoundation.h
 */
@protocol CollectionProtocol
@required

/**
 * @brief Returns the number of elements in the collection.
 * @return The number of elements in the collection.
 */
- (unsigned int)count;

/**
 * @brief Returns YES if the collection contains the specified object.
 * @param object The object to check for containment.
 * @return YES if the collection contains the specified object, NO otherwise.
 */
- (BOOL)containsObject:(id)object;

@end
