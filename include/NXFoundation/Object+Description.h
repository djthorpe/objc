/**
 * @file Object+Description.h
 * @brief Adds a description method to the Object class.
 *
 * This file defines a category on the Object class to provide a
 * method for obtaining a string representation of an object or class.
 */

/**
 * @category Object(Description)
 * @brief A category that adds a description method to the Object class.
 *
 * This category provides a standardized way to get a string
 * representation of any object instance or class, which is useful for
 * debugging and logging purposes.
 *
 * \headerfile Object+Description.h NXFoundation/NXFoundation.h
 */
@interface Object (Description)

/**
 * @brief Returns a string that represents the instance.
 * @return A string that describes the receiver.
 */
- (NXString *)description;

/**
 * @brief Returns a string that represents the class.
 * @return A string that describes the receiver.
 */
+ (NXString *)description;

@end
