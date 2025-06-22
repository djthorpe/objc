#include <stdio.h>
#include <objc-gcc/objc.h>
#include "api.h"

// This list contains all modules currently loaded into the runtime.
#define OBJC_MODULES_VERSION 8
#define OBJC_MAX_MODULES 100
const int max_modules = OBJC_MAX_MODULES;
static struct objc_module_t* modules[OBJC_MAX_MODULES];
static int modules_count = 0;

// Private function declarations
void __objc_class_init();
void __objc_module_register(struct objc_module_t* );
void __objc_class_register(struct objc_class_t* );

///////////////////////////////////////////////////////////////////////////////

void __objc_exec_class(struct objc_module_t *module) {
    if (modules_count >= max_modules) {
        panicf("Maximum number of modules reached.");
    }
    modules[modules_count++] = module;
}

void objc_init() {
    static BOOL init;
    if (init) {
        return; // Already initialized
    }
    init = YES;

    // initialize the modules
    __objc_class_init();
    for (int i = 0; i < modules_count; i++) {
        __objc_module_register(modules[i]);
    }
}

void __objc_module_register(struct objc_module_t *module) {
    if (module == NULL || module->version != OBJC_MODULES_VERSION) {
        panicf("Invalid module version: %lu", module ? module->version : 0);
    }

    int j = 0;
    for (int i = 0; i < module->symtab->class_count; i++) {
        struct objc_class_t *cls = (struct objc_class_t *)module->symtab->defs[j++];
        __objc_class_register(cls);
    }
    for (int i = 0; i < module->symtab->category_count; i++) {
        struct objc_category_t *cat = (struct objc_category_t *)module->symtab->defs[j++];
        printf("   Category %s\n", cat->name);
    }
    do {
        // This is a static class instance
        id inst = (id)module->symtab->defs[j++];
        if (inst == NULL) {
            break; // No more static instances
        }
        printf("   Static instance %p\n", inst);
    } while(true);
}
