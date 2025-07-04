/**
 * @file Object.h
 * @brief The root class of all Objective-C classes.
 *
 * This class provides basic memory management and introspection capabilities.
 */
#pragma once

/**
 * @brief The root class of all Objective-C classes.
 *
 * This class is the base class for all objects in Objective-C.
 * It provides basic memory management and introspection capabilities.
 * 
 * \headerfile Object.h objc/objc.h
 */
OBJC_ROOT_CLASS
@interface Object {
    /**
     * @var isa
     * @brief A pointer to the object's class structure.
     *
     * This instance variable holds a reference to the class to which the
     * object belongs. It is a fundamental part of the Objective-C object model,
     * enabling dynamic dispatch and introspection.
     */
    Class isa;
}

/**
 * @brief Performs one-time initialization for the class.
 *
 * This method is called automatically by the Objective-C runtime before the class 
 * receives messages. 
 *
 * Subclasses may override this method to perform initialization tasks like setting
 * up class variables or initializing static data structures. If subclasses implement
 * this method, they are called first, and may be called multiple times.
 */
+(void) initialize;

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
 * @return The initialized object.
 *
 * Subclasses should override this method to perform custom
 * initialization, and free resources if the initialization fails.
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
 * @return The superclass of the receiver, or Nil if it is a root class.
 */
-(Class) superclass;

/**
 * @brief Returns the superclass of the class.
 * @return The superclass of the receiver, or Nil if it is a root class.
 */
+(Class) superclass;

/**
 * @brief Returns the name of the class.
 * @return A C-string containing the name of the class.
 */
+(const char* ) name;

/**
 * @brief Compares the receiver to another object for equality.
 * @param anObject The object to compare with the receiver.
 * @return YES if the objects are equal, otherwise NO.
 *
 * When comparing two objects, this method should be overridden to
 * compare the contents of the objects.
 */
-(BOOL) isEqual:(id)anObject;

/**
 * @brief Returns a Boolean value that indicates whether the receiver is an instance of a given class.
 * @param cls A class object.
 * @return YES if the receiver is an instance of cls or an instance of any class that inherits from cls, otherwise NO.
 */
-(BOOL) isKindOfClass:(Class)cls;

@end
