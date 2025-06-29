#pragma once

// Opaque types
typedef struct objc_object* id;
typedef struct objc_selector* SEL;
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

// Objects
Class object_getClass (id object);