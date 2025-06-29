#include <stdio.h>
#include <string.h>
#include <objc/objc.h>
#include "api.h"
#include "class.h"

#define STATICS_TABLE_SIZE 32
static struct objc_static_instances_list *statics_table[STATICS_TABLE_SIZE+1];
static void __objc_statics_load_list(struct objc_static_instances_list *list);

///////////////////////////////////////////////////////////////////////////////

void __objc_statics_init() {
    static BOOL init;
    if (init) {
        return; // Already initialized
    }
    init = YES;
    for (int i = 0; i <= STATICS_TABLE_SIZE; i++) {
        statics_table[i] = NULL;
    }
}

void __objc_statics_register(struct objc_static_instances_list *statics) {
    if (statics == NULL  || statics->class_name == NULL ) {
        return;
    }
    printf("__objc_statics_register [%s]\n", statics->class_name);
    for(int i = 0; i < STATICS_TABLE_SIZE; i++) {
        if (statics_table[i] == statics || statics_table[i] == NULL) {
            statics_table[i] = statics;
            return;
        }
    }
    panicf("Static instances table is full, cannot register class: %s", statics->class_name);
}

BOOL __objc_statics_load() {
    static BOOL init;
    if (init) {
        return NO; // Already initialized
    }
    init = YES;

    // Replace class name with resolved class
    for(int i = 0; i < STATICS_TABLE_SIZE; i++) {
        struct objc_static_instances_list *list = statics_table[i];
        if (list == NULL) {
            continue; // No static instances in this slot
        }
        __objc_statics_load_list(list);
    }

    return YES;
}

static void __objc_statics_load_list(struct objc_static_instances_list *list) {
    // Lookup the class by name
    objc_class_t* cls = __objc_lookup_class(list->class_name);
    if (cls == NULL) {
        panicf("Static instances class '%s' not found", list->class_name);
        return;
    }

    // Register each static instance
    for (id *instance=list->instances ; *instance != nil ; instance++) {
		(*instance)->isa = cls;
	}
}
