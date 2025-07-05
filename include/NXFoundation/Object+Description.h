/**
 * @file Object+Description.h
 * @brief Adds a description method to the root Object class.
 *
 * This file defines a category on the Object class to provide a
 * method for obtaining a string representation of an object.
 *
 * \headerfile Object+Description.h NXFoundation/NXFoundation.h
 */
#include <NXFoundation/NXFoundation.h>

/**
 * @category Object(Description)
 * @brief A category that adds a description method to the Object class.
 *
 * This category provides a standardized way to get a string
 * representation of any object, which is useful for debugging
 * and logging purposes.
 */
@interface Object (Description)

/**
 * @brief Returns a string that represents the contents of the receiver.
 * @return A string that describes the receiver.
 */
-(NXString* ) description;

@end