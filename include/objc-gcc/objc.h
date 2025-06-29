#pragma once
#include "runtime.h"
#include "assert.h"
#include "panicf.h"

#if __OBJC__
#include "Object.h"
#include "Protocol.h"
#endif // __OBJC__

/**
 * Looks up the class with the specified name. Returns Nil if the class is not found.
 */
Class objc_lookupClass(const char* name);

/**
 * Returns the name of the class, or NULL if the class is Nil.
 */
const char *class_getName(Class cls);

/**
 * Returns the name of the class of the object, or NULL if the object is nil.
 */
const char *object_getClassName(id obj);

/**
 * Returns the class of the object. Returns Nil if the object is nil.
 */
Class object_getClass (id object);

/**
  * Sets the class of the object to the specified class.
  */
void object_setClass(id object, Class cls);

/**
 * Returns the size of an instance of the named class, in bytes. Returns 0 if the class is Nil
 */
size_t class_getInstanceSize(Class cls);
