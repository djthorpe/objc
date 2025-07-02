#pragma once
#include <stddef.h>

// Opaque types
typedef struct objc_object* id;
typedef const struct objc_selector *SEL;
typedef struct objc_class* Class;
typedef id (*IMP)(id, SEL, ...);
typedef struct objc_method* Method;
//typedef struct objc_property* Property;

#define nil ((id)0)
#define Nil ((Class)0)

// Booleans
#include <stdbool.h>
typedef bool BOOL;
#define YES true
#define NO false

// Define the root class attribute
#if __has_attribute(objc_root_class)
#define OBJC_ROOT_CLASS __attribute__((objc_root_class))
#else
#define OBJC_ROOT_CLASS
#endif

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
 * Returns the superclass of an instance, or Nil if it is a root class
 */
Class object_getSuperclass(id obj);

/**
 * Returns the superclass of a class, or Nil if it is a root class
 */
Class class_getSuperclass(Class cls);

/**
 * Returns the size of an instance of the named class, in bytes. Returns 0 if the class is Nil
 */
size_t class_getInstanceSize(Class cls);

/**
 * Returns YES if the class responds to the specified selector, NO otherwise.
 */
BOOL class_metaclassRespondsToSelector(Class cls, SEL sel);

/**
 * Returns YES if the class instance responds to the specified selector, NO otherwise.
 */
BOOL class_respondsToSelector(Class cls, SEL sel);
