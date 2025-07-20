#pragma once
#include "api.h"
#include <objc/objc.h>

///////////////////////////////////////////////////////////////////////////////////
// TYPES

typedef struct objc_class objc_class_t;

///////////////////////////////////////////////////////////////////////////////////
// METHODS

/*
 * Initializes the Objective-C runtime class table
 */
void __objc_class_init();

/*
 * Register a class in the Objective-C runtime.
 */
void __objc_class_register(objc_class_t *cls);

/*
 * Register a class category in the Objective-C runtime.
 */
void __objc_class_category_register(struct objc_category *cat);

/*
 * Lookup a class by name in the Objective-C runtime.
 * Returns Nil if the class is not found.
 */
Class objc_lookup_class(const char *name);

/**
 * Register a list of methods for a class.
 * This function registers all methods in the method list, for the named class.
 */
void __objc_class_register_method_list(objc_class_t *cls,
                                       struct objc_method_list *ml);
