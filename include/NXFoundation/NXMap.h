
/**
 * @file NXMap.h
 * @brief Defines a map class for storing key-value pairs, where keys are
 * strings.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The NXMap class
 * @ingroup Foundation
 * @headerfile NXMap.h NXFoundation/NXFoundation.h
 *
 * NXMap represents a map that can store key-value pairs, where keys are
 * strings and values are arbitrary objects.
 *
 * The map uses an optimized hash table implementation for efficient storage
 * and retrieval of key-value pairs. Keys must be string objects that implement
 * the NXConstantStringProtocol.
 *
 * Objects stored in the map are retained and will be released when the map
 * is deallocated, when an object is removed,  or when removeAllObjects is
 * called.
 *
 */
@interface NXMap : NXObject <JSONProtocol, CollectionProtocol> {
@private
  void *_data;         ///< Pointer to the map data
  unsigned int _count; ///< Current number of key-value pairs in the map
  size_t _capacity;    ///< Original capacity of the map data
}

/**
 * @brief Returns a new empty NXMap instance.
 */
+ (NXMap *)new;

/**
 * @brief Returns a new NXMap instance with the specified initial capacity.
 * @param capacity The initial capacity of the map.
 * @return A new NXMap instance with the specified capacity.
 */
+ (NXMap *)mapWithCapacity:(size_t)capacity;

/**
 * @brief Creates and returns a new NXMap initialized with alternating objects
 * and keys.
 * @param firstObject The first object to store in the map, followed by its key,
 *                    then the next object, its key, and so on. The list must be
 *                    terminated with nil.
 * @param ... A nil-terminated list of alternating objects and keys.
 * @return A new autoreleased NXMap instance containing the specified key-value
 * pairs, or nil if an error occurs during creation.
 *
 * This method creates a new map by taking pairs of arguments where each odd
 * argument is an object and each even argument is its corresponding key. The
 * argument list must be terminated with nil. Keys must implement the
 * NXConstantStringProtocol.
 *
 * Example usage:
 * @code
 * NXMap *map = [NXMap mapWithObjectsAndKeys:
 *     @"value1", @"key1",
 *     @"value2", @"key2",
 *     nil];
 * @endcode
 */
+ (NXMap *)mapWithObjectsAndKeys:(id)firstObject,
                                 ... OBJC_REQUIRES_NIL_TERMINATION;

/**
 * @brief Creates and returns a new NXMap initialized with objects from one
 * array and corresponding keys from another array.
 * @param objects An NXArray containing the objects to store in the map.
 *                Must not be nil and must contain the same number of elements
 * as keys.
 * @param keys An NXArray containing the keys to associate with the objects.
 *             Must not be nil, must contain the same number of elements as
 * objects, and all elements must implement the NXConstantStringProtocol.
 * @return A new autoreleased NXMap instance containing the key-value pairs
 * formed by pairing elements from the two arrays, or nil if an error occurs.
 *
 * This method creates a new map by pairing elements at corresponding indices
 * from the two input arrays. The first element of the objects array is paired
 * with the first element of the keys array, the second with the second, and so
 * on.
 *
 * Example usage:
 * @code
 * NXArray *objects = [NXArray arrayWithObjects:@"value1", @"value2", nil];
 * NXArray *keys = [NXArray arrayWithObjects:@"key1", @"key2", nil];
 * NXMap *map = [NXMap mapWithObjects:objects forKeys:keys];
 * @endcode
 *
 * Both arrays must contain the same number of elements. If they differ
 * in size, the behavior is undefined.
 */
+ (NXMap *)mapWithObjects:(NXArray *)objects forKeys:(NXArray *)keys;

/**
 * @brief Returns the number of elements in the map.
 * @return The number of elements in the map.
 */
- (unsigned int)count;

/**
 * @brief Returns the capacity of the map.
 * @return The maximum number of elements the map can hold before resizing.
 */
- (size_t)capacity;

/**
 * @brief Returns an array containing all keys stored in the map.
 * @return An NXArray containing all keys in the map, or an empty array
 *         if the map is empty.
 *
 * The order of keys in the returned array is not guaranteed to match
 * the order of insertion. The array contains only the keys stored in the
 * map, not the associated objects.
 */
- (NXArray *)allKeys;

/**
 * @brief Returns an array containing all objects stored in the map.
 * @return An NXArray containing all objects in the map, or an empty array
 *         if the map is empty.
 *
 * The order of objects in the returned array is not guaranteed to match
 * the order of insertion. The array contains only the values stored in the
 * map, not the keys.
 */
- (NXArray *)allObjects;

/**
 * @brief Removes all objects from the map.
 *
 * This method effectively clears all key-value pairs from the map.
 * All previously stored objects are released, and any references to them
 * become invalid. The map remains fully functional after this operation
 * and can accept new key-value pairs immediately.
 */
- (void)removeAllObjects;

/**
 * @brief Stores an object in the map with the specified key.
 * @param anObject The object to store in the map. Must be non-nil.
 * @param key The string key to associate with the object. Must be non-nil
 *            and implement both the NXConstantStringProtocol and
 * RetainProtocol.
 * @return YES if the object was successfully stored, NO if the operation
 *         failed (e.g., due to memory allocation failure).
 *
 * If a key already exists in the map, the new object will overwrite
 * the existing one. The object is retained by the map.
 */
- (BOOL)setObject:(id)anObject
           forKey:(id<NXConstantStringProtocol, RetainProtocol>)key;

/**
 * @brief Retrieves an object from the map by its key.
 * @param key The string key to look up. Must be non-nil and implement
 *            both the NXConstantStringProtocol and RetainProtocol.
 * @return The object associated with the given key, or nil if the key
 *         is not found in the map.
 */
- (id)objectForKey:(id<NXConstantStringProtocol, RetainProtocol>)key;

/**
 * @brief Removes an object from the map by its key.
 * @param key The string key of the object to remove. Must be non-nil and
 *            implement both the NXConstantStringProtocol and RetainProtocol.
 * @return YES if an object was found and successfully removed, NO if the key
 *         was not found in the map or if the operation failed.
 *
 * When an object is successfully removed, it is released by the map.
 *       The map's count is decremented by one. If the key is not found,
 *       the map remains unchanged.
 */
- (BOOL)removeObjectForKey:(id<NXConstantStringProtocol, RetainProtocol>)key;

@end
