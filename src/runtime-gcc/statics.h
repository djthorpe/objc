#pragma once

/*
 * Initializes the static instances for the Objective-C runtime.
 */
void __objc_statics_init();

/*
 * Registers static instances from the specified list.
 */
void __objc_statics_register(struct objc_static_instances_list* statics);

/*
 * Loads static instances from the specified list.
 * This function replaces class names with resolved class pointers
 * or returns NO if the loading was already done.
 */
BOOL __objc_statics_load();
