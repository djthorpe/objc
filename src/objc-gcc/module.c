#include <stdio.h>
#include <objc-gcc/objc.h>
#include "api.h"
#include "hash.h"
#include "class.h"

// Private function declarations
static void __objc_module_register(struct objc_module* );

///////////////////////////////////////////////////////////////////////////////

void __objc_exec_class(struct objc_module *module) {
    __objc_class_init();
    __objc_hash_init();
    __objc_module_register(module);
}

static void __objc_module_register(struct objc_module* module) {
    if (module == NULL || module->version != OBJC_ABI_VERSION) {
        panicf("Invalid abi version: %lu", module ? module->version : 0);
        return;
    }

    int j = 0;
    for (int i = 0; i < module->symtab->class_count; i++) {
        objc_class_t *cls = (objc_class_t *)module->symtab->defs[j++];
        __objc_class_register(cls);
    }
    for (int i = 0; i < module->symtab->category_count; i++) {
        struct objc_category *cat = (struct objc_category *)module->symtab->defs[j++];
        __objc_class_category_register(cat);
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
