/**
 * @file runtime.h
 * @brief Defines the core types and functions of the Objective-C runtime.
 *
 * This file provides the fundamental data structures and functions
 * that constitute the Objective-C runtime. It includes definitions for objects,
 * classes, selectors, and methods, as well as functions for introspection
 * and message dispatch.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** @brief A pointer to an instance of a class.
 * @ingroup objc
 */
typedef struct objc_object *id;

/** @brief A pointer to a method selector.
 * @ingroup objc
 */
typedef const struct objc_selector *SEL;

/** @brief A pointer to a class definition.
 * @ingroup objc
 */
typedef struct objc_class *Class;

/** @brief A pointer to a method implementation.
 * @ingroup objc
 */
typedef id (*IMP)(id, SEL, ...);

/** @brief A pointer to a method.
 * @ingroup objc
 */
typedef struct objc_method *Method;

/** @brief An instance of a protocol.
 * @ingroup objc
 */
typedef struct objc_protocol objc_protocol_t;

/** @brief A null object pointer.
 * @ingroup objc
 */
#define nil ((id)0)

/** @brief A null class pointer.
 * @ingroup objc
 */
#define Nil ((Class)0)

// Booleans

/** @brief A Boolean value.
 * @ingroup objc
 */
typedef bool BOOL;

/** @brief The Boolean value `true`.
 * @ingroup objc
 */
#define YES true

/** @brief The Boolean value `false`.
 * @ingroup objc
 */
#define NO false

/**
 * @def OBJC_ROOT_CLASS
 * @ingroup objc
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
 * @ingroup objc
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
 * @def OBJC_REQUIRES_NIL_TERMINATION
 * @ingroup objc
 * @brief A macro to indicate that a method requires a nil-terminated list of
 * arguments.
 *
 * This macro is used in method declarations to specify that the method
 * accepts a variable number of arguments that must be terminated with `nil`.
 */
#define OBJC_REQUIRES_NIL_TERMINATION

/**
 * @brief Looks up a class by name.
 * @ingroup objc
 * @param name The name of the class to look up.
 * @return The class object, or `Nil` if the class is not found.
 */
Class objc_lookupClass(const char *name);

/**
 * @brief Returns the name of a class.
 * @ingroup objc
 * @param cls The class to inspect.
 * @return A C-string containing the name of the class, or `NULL` if `cls` is
 * `Nil`.
 */
const char *class_getName(Class cls);

/**
 * @brief Returns the name of an object's class.
 * @ingroup objc
 * @param obj The object to inspect.
 * @return A C-string containing the name of the object's class, or `NULL` if
 * `obj` is `nil`.
 */
const char *object_getClassName(id obj);

/**
 * @brief Returns the class of an object.
 * @ingroup objc
 * @param object The object to inspect.
 * @return The class of the object, or `Nil` if the object is `nil`.
 */
Class object_getClass(id object);

/**
 * @brief Sets the class of an object.
 * @ingroup objc
 * @param object The object to modify.
 * @param cls The new class for the object.
 */
void object_setClass(id object, Class cls);

/**
 * @brief Checks if an instance's class responds to a selector.
 * @ingroup objc
 * @param object The object to inspect.
 * @param sel The selector to check.
 * @return `YES` if instances of the class respond to the selector, `NO`
 * otherwise.
 */
BOOL object_respondsToSelector(id object, SEL sel);

/**
 * @brief Returns the superclass of an object.
 * @ingroup objc
 * @param obj The object to inspect.
 * @return The superclass of the object, or `Nil` if it is a root class.
 */
Class object_getSuperclass(id obj);

/**
 * @brief Returns the superclass of a class.
 * @ingroup objc
 * @param cls The class to inspect.
 * @return The superclass of the class, or `Nil` if it is a root class.
 */
Class class_getSuperclass(Class cls);

/**
 * @brief Checks if an instance class matches, or subclass of another class.
 * @ingroup objc
 * @param object The object to inspect.
 * @param cls The class to compare against.
 * @return `YES` if `object` class matches or is a subclass of `cls`, `NO`
 * otherwise.
 */
BOOL object_isKindOfClass(id object, Class cls);

/**
 * @brief Returns the size of an instance of a class.
 * @ingroup objc
 * @param cls The class to inspect.
 * @return The size of an instance of the class in bytes, or 0 if the class is
 * `Nil`.
 */
size_t class_getInstanceSize(Class cls);

/**
 * @brief Checks if a class object responds to a selector.
 * @ingroup objc
 * @param cls The class to inspect.
 * @param sel The selector to check.
 * @return `YES` if the class responds to the selector, `NO` otherwise.
 */
BOOL class_metaclassRespondsToSelector(Class cls, SEL sel);

/**
 * @brief Checks if an instance of a class responds to a selector.
 * @ingroup objc
 * @param cls The class to inspect.
 * @param sel The selector to check.
 * @return `YES` if instances of the class respond to the selector, `NO`
 * otherwise.
 */
BOOL class_respondsToSelector(Class cls, SEL sel);

/**
 * @brief Returns the name of a selector.
 * @ingroup objc
 * @param sel The selector to inspect.
 * @return A C-string representing the selector's name.
 */
const char *sel_getName(SEL sel);

/**
 * @brief Returns the name of a protocol.
 * @ingroup objc
 * @param protocol The protocol to inspect.
 * @return A C-string containing the name of the protocol, or `NULL` if
 * `protocol` is `NULL`.
 */
const char *proto_getName(objc_protocol_t *protocol);

/**
 * @brief Checks if a protocol conforms to another protocol.
 * @ingroup objc
 * @param protocol The protocol to test for conformance.
 * @param otherProtocol The protocol to check conformance against.
 * @return `YES` if `protocol` conforms to `otherProtocol`, `NO` otherwise.
 */
BOOL proto_conformsTo(objc_protocol_t *protocol,
                      objc_protocol_t *otherProtocol);

/**
 * @brief Checks if a class conforms to a protocol.
 * @ingroup objc
 * @param cls The class to test for conformance.
 * @param otherProtocol The protocol to check conformance against.
 * @return `YES` if methods of a `cls` conforms to `otherProtocol`, `NO`
 * otherwise.
 */
BOOL class_conformsTo(Class cls, objc_protocol_t *otherProtocol);
