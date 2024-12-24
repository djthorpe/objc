#pragma once

// Opaque types
typedef struct objc_object* id;
typedef const char* SEL;
typedef struct objc_class* Class;
typedef id (*IMP)(id, SEL, ...);
typedef struct objc_method* Method;
typedef struct objc_property* Property;

#define nil ((id)0)
#define Nil ((Class)0)

// Booleans
#include <stdbool.h>
typedef bool BOOL;
#define YES true
#define NO false

// Class and Function attributes
#define NS_ROOT_CLASS __attribute__((objc_root_class))
