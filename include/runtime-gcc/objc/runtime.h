/**
 * @file runtime.h
 * @brief Defines the core types and functions of the Objective-C runtime.
 * @details This file provides the fundamental data structures and functions that
 * constitute the Objective-C runtime. It includes definitions for objects,
 * classes, selectors, and methods, as well as functions for introspection
 * and message dispatch.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief A pointer to an instance of a class. */
typedef struct objc_object* id;
/** @brief A pointer to a method selector. */
typedef const struct objc_selector *SEL;
/** @brief A pointer to a class definition. */
typedef struct objc_class* Class;
/** @brief A pointer to a method implementation. */
typedef id (*IMP)(id, SEL, ...);
/** @brief A pointer to a method. */
typedef struct objc_method* Method;
//typedef struct objc_property* Property;

/** @brief A null object pointer. */
#define nil ((id)0)
/** @brief A null class pointer. */
#define Nil ((Class)0)

// Booleans
/** @brief A Boolean value. */
typedef bool BOOL;
/** @brief The Boolean value `true`. */
#define YES true
/** @brief The Boolean value `false`. */
#define NO false

/**
 * @def OBJC_ROOT_CLASS
 * @brief A macro to declare a class as a root class.
 *
 * This macro uses the `objc_root_class` attribute if it is available,
 * which allows a class to be defined without a superclass.
 */
#if __has_attribute(objc_root_class)
#define OBJC_ROOT_CLASS __attribute__((objc_root_class))
#else
#define OBJC_ROOT_CLASS
#endif

/**
 * @def OBJC_UNUSED
 * @brief A macro to declare a method parameter is unused.
 *
 * This macro uses the `unused` attribute if it is available,
 * which allows a method parameter to be marked as unused.
 */
#if __has_attribute(unused)
#define OBJC_UNUSED __attribute__((unused))
#else
#define OBJC_UNUSED
#endif

/**
 * @brief Looks up a class by name.
 * @param name The name of the class to look up.
 * @return The class object, or `Nil` if the class is not found.
 */
Class objc_lookupClass(const char* name);

/**
 * @brief Returns the name of a class.
 * @param cls The class to inspect.
 * @return A C-string containing the name of the class, or `NULL` if `cls` is `Nil`.
 */
const char *class_getName(Class cls);

/**
 * @brief Returns the name of an object's class.
 * @param obj The object to inspect.
 * @return A C-string containing the name of the object's class, or `NULL` if `obj` is `nil`.
 */
const char *object_getClassName(id obj);

/**
 * @brief Returns the class of an object.
 * @param object The object to inspect.
 * @return The class of the object, or `Nil` if the object is `nil`.
 */
Class object_getClass (id object);

/**
 * @brief Sets the class of an object.
 * @param object The object to modify.
 * @param cls The new class for the object.
 */
void object_setClass(id object, Class cls);

/**
 * @brief Returns the superclass of an object.
 * @param obj The object to inspect.
 * @return The superclass of the object, or `Nil` if it is a root class.
 */
Class object_getSuperclass(id obj);

/**
 * @brief Returns the superclass of a class.
 * @param cls The class to inspect.
 * @return The superclass of the class, or `Nil` if it is a root class.
 */
Class class_getSuperclass(Class cls);

/**
 * @brief Checks if an instance class matches, or subclass of another class.
 * @param object The object to inspect.
 * @param cls The class to compare against.
 * @return `YES` if `object` class matches or is a subclass of `cls`, `NO` otherwise.
 */
BOOL object_isKindOfClass(id object, Class cls);

/**
 * @brief Returns the size of an instance of a class.
 * @param cls The class to inspect.
 * @return The size of an instance of the class in bytes, or 0 if the class is `Nil`.
 */
size_t class_getInstanceSize(Class cls);

/**
 * @brief Checks if a class object responds to a selector.
 * @param cls The class to inspect.
 * @param sel The selector to check.
 * @return `YES` if the class responds to the selector, `NO` otherwise.
 */
BOOL class_metaclassRespondsToSelector(Class cls, SEL sel);

/**
 * @brief Checks if an instance of a class responds to a selector.
 * @param cls The class to inspect.
 * @param sel The selector to check.
 * @return `YES` if instances of the class respond to the selector, `NO` otherwise.
 */
BOOL class_respondsToSelector(Class cls, SEL sel);

/**
 * @brief Returns the name of a selector.
 * @param sel The selector to inspect.
 * @return A C-string representing the selector's name.
 */
const char* sel_getName(SEL sel);