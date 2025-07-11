/**
 * @file NXObject+Description.h
 * @brief Adds a description method to the NXObject class.
 *
 * This file defines a category on the NXObject class to provide a
 * method for obtaining a string representation of an object or class.
 */
#include "NXObject.h"

/**
 * @category NXObject(Description)
 * @brief A category that adds a description method to the NXObject class.
 *
 * This category provides a standardized way to get a string
 * representation of any object instance or class, which is useful for
 * debugging and logging purposes.
 *
 * \headerfile NXObject+Description.h NXFoundation/NXFoundation.h
 */
@interface NXObject (Description)

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
