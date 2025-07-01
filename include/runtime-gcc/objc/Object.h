#pragma once

/*
 * The root class of all Objective-C classes.
 * This class is the base class for all objects in Objective-C.
 * It provides basic memory management and introspection capabilities.
 */
OBJC_ROOT_CLASS
@interface Object {
    Class isa;
}

/*
 * Allocate a new class instance. Returns a pointer to the instance, or nil
 * if the allocation failed
 */
+(id) alloc;

/*
 * Free resources for an existing instance.
 */
-(void) dealloc;

/*
 * Initialize the instance, after allocation. Ultimately this method should
 * be overridden by subclasses to perform custom initialization, and free resources
 * if the initialization fails.
 */
-(id) init;

/*
 * Returns the class of the instance.
 */
-(Class) class;

/*
 * Returns the class (itself).
 */
+(Class) class;

/*
 * Returns the superclass of the instance.
 */
-(Class) superclass;

/*
 * Returns the superclass of the class.
 */
+(Class) superclass;

/*
 * Returns the name of the instance class.
 */
-(const char* ) name;

/*
 * Returns the name of the class.
 */
+(const char* ) name;

/*
 * Returns true if one object is equal to another. When comparing two objects,
 * this method should be overridden to compare the contents of the objects.
 */
-(BOOL) isEqual:(id)anObject;

@end
