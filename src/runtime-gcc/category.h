#pragma once
#include "api.h"

/*
 * Initializes the category instances for the Objective-C runtime.
 */
void __objc_category_init();

/*
 * Registers a category.
 */
void __objc_category_register(struct objc_category *category);

/*
 * Loads categories.
 * This function replaces methods from a category
 * or returns NO if the loading was already done.
 */
BOOL __objc_category_load();
