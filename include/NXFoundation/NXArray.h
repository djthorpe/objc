
/**
 * @file NXArray.h
 * @brief Defines an array class for storing ordered objects.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The NXArray class
 *
 * NXArray represents an array that can store ordered objects.
 *
 * \headerfile NXArray.h NXFoundation/NXFoundation.h
 */
@interface NXArray : NXObject <JSONProtocol, CollectionProtocol> {
@private
  void **_data;   ///< Pointer to the array data
  size_t _length; ///< Current number of elements in the array
  size_t _cap;    ///< Capacity of the array data
}

/**
 * @brief Returns a new empty NXArray instance.
 */
+ (NXArray *)new;

/**
 * @brief Returns a new NXArray instance with the specified capacity.
 * @param capacity The initial capacity of the array.
 * @return A new NXArray instance with the specified capacity.
 */
+ (NXArray *)arrayWithCapacity:(size_t)capacity;

/**
 * @brief Initializes an array with a variadic list of objects.
 * @param firstObject The first object to add to the array, followed by
 * additional objects.
 * @return An initialized NXArray instance containing the specified objects, or
 * nil on failure.
 *
 * This method takes a variadic list of objects terminated by nil. Objects are
 * added to the array in the order they appear in the argument list.
 */
- (id)initWithObjects:(id<RetainProtocol>)firstObject, ...;

/**
 * @brief Returns a new NXArray instance with the specified objects.
 * @param firstObject The first object to add to the array, followed by
 * additional objects.
 * @return A new NXArray instance containing the specified objects, or nil on
 * failure.
 *
 * This method takes a variadic list of objects terminated by nil. Objects are
 * added to the array in the order they appear in the argument list. The
 * returned array is autoreleased.
 */
+ (NXArray *)arrayWithObjects:(id<RetainProtocol>)firstObject, ...;

/**
 * @brief Returns the number of elements in the array.
 * @return The number of elements in the array.
 */
- (unsigned int)count;

/**
 * @brief Returns the capacity of the array.
 * @return The total allocated size of the array, including any unused elements.
 *
 * This method returns the current capacity of the array's internal storage,
 * which may be larger than the actual number of elements in the array. This is
 * useful for understanding how much space is available for adding new elements
 * without requiring reallocation.
 */
- (size_t)capacity;

/**
 * @brief Returns the first object in the array.
 * @return The first object in the array, or nil if the array is empty.
 */
- (id)firstObject;

/**
 * @brief Returns the last object in the array.
 * @return The last object in the array, or nil if the array is empty.
 */
- (id)lastObject;

/**
 * @brief Returns YES if the collection contains the specified object.
 * @param object The object to check for containment.
 * @return YES if the collection contains the specified object, NO otherwise.
 *
 * This method recursively checks each element in the collection for equality
 * with the specified object. It will return YES if any element matches,
 * including those in nested arrays and maps. If you wish to check for an
 * object's presence as an element in the array, use the `indexForObject:`
 * method instead.
 */
- (BOOL)containsObject:(id)object;

/**
 * @brief Returns the lowest index for an object equivalent to the specified
 * object.
 * @param index The object to find in the array.
 * @return The index of the object in the array, or NXNotFound if the object
 *
 * This method searches the array from the beginning to the end and returns the
 * lowest index for an object equivalent to the specified object. Two objects
 * are considered equivalent if their `isEqual:` method returns YES.
 */
- (id)objectAtIndex:(unsigned int)index;

/**
 * @brief Returns the index for the specified object.
 * @param object The object to find in the array.
 * @return The index of the object in the array, or NXNotFound if the object
 * is not found.
 */
- (unsigned int)indexForObject:(id<ObjectProtocol>)object;

/**
 * @brief Appends an object to the end of the array.
 * @param object The object to append to the array.
 * @return YES if the object was successfully appended, NO otherwise.
 */
- (BOOL)append:(id<RetainProtocol, ObjectProtocol>)object;

/**
 * @brief Inserts an object at the specified index in the array.
 * @param object The object to insert into the array.
 * @param index The index at which to insert the object.
 * @return YES if the object was successfully inserted, NO otherwise.
 *
 * This method shifts existing elements at and after the specified index
 * to make room for the new object. If the index is greater than the
 * current count, an exception should be thrown.
 */
- (BOOL)insert:(id<RetainProtocol, ObjectProtocol>)object
       atIndex:(unsigned int)index;

/**
 * @brief Removes the first occurrence of the specified object from the array.
 * @param object The object to remove from the array.
 * @return YES if the object was found and successfully removed, NO otherwise.
 *
 * This method searches the array from the beginning to find the first
 * occurrence of an object that is equal to the specified object (using the
 * `isEqual:` method). If found, the object is removed and all subsequent
 * elements are shifted down to fill the gap. The array's count is decremented
 * by one. If the object is not found in the array, the method returns NO and
 * the array remains unchanged.
 */
- (BOOL)remove:(id<RetainProtocol>)object;

/**
 * @brief Removes the object at the specified index from the array.
 * @param index The index of the object to remove.
 * @return YES if the object was successfully removed, NO otherwise.
 *
 * This method removes the object at the specified index and shifts all
 * subsequent elements down by one position to fill the gap. The array's count
 * is decremented by one.
 */
- (BOOL)removeObjectAtIndex:(unsigned int)index;

/**
 * @brief Removes all objects from the array.
 *
 *
 * This method clears the array by releasing all objects and resetting the
 * internal data structure. The array's count is set to zero and its capacity
 * remains unchanged.
 */
- (void)removeAllObjects;

@end
