/**
 * @file Object.h
 * @brief The root class of all Objective-C classes.
 * @details This class provides basic memory management and introspection capabilities.
 */
#pragma once

/**
 * @brief The root class of all Objective-C classes.
 * @details This class is the base class for all objects in Objective-C.
 * It provides basic memory management and introspection capabilities.
 */
OBJC_ROOT_CLASS
@interface Object {
    Class isa;
}

/**
 * @brief Allocate a new class instance.
 * @return A pointer to the instance, or nil if the allocation failed.
 */
+(id) alloc;

/**
 * @brief Free resources for an existing instance.
 */
-(void) dealloc;

/**
 * @brief Initialize the instance, after allocation.
 * @details Subclasses should override this method to perform custom
 * initialization, and free resources if the initialization fails.
 * @return The initialized object.
 */
-(id) init;

/**
 * @brief Returns the class of the instance.
 * @return The class of the receiver.
 */
-(Class) class;

/**
 * @brief Returns the class object.
 * @return The class object for the receiver.
 */
+(Class) class;

/**
 * @brief Returns the superclass of the instance.
 * @return The superclass of the receiver.
 */
-(Class) superclass;

/**
 * @brief Returns the superclass of the class.
 * @return The superclass of the receiver.
 */
+(Class) superclass;

/**
 * @brief Returns the name of the instance's class.
 * @return A C-string containing the name of the receiver's class.
 */
-(const char* ) name;

/**
 * @brief Returns the name of the class.
 * @return A C-string containing the name of the receiver.
 */
+(const char* ) name;

/**
 * @brief Compares the receiver to another object for equality.
 * @param anObject The object to compare with the receiver.
 * @return YES if the objects are equal, otherwise NO.
 * @details When comparing two objects, this method should be overridden to
 * compare the contents of the objects.
 */
-(BOOL) isEqual:(id)anObject;

/**
 * @brief Returns a Boolean value that indicates whether the receiver implements
 * or inherits a method that can respond to a specified message.
 * @param aSel A selector that identifies a message.
 * @return YES if the receiver responds to the aSel message, otherwise NO.
 */
//-(BOOL) respondsTo:(SEL)aSel;

/**
 * @brief Returns a Boolean value that indicates whether the class can respond
 * to a specified message.
 * @param aSel A selector that identifies a message.
 * @return YES if the class responds to the aSel message, otherwise NO.
 */
//+(BOOL) respondsTo:(SEL)aSel;

@end
