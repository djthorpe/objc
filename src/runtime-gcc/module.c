#include <stdio.h>
#include <stdlib.h>

#include <objc/objc.h>
#include "api.h"
#include "hash.h"
#include "hexdump.h"
#include "class.h"
#include "statics.h"

// Private function declarations
static void __objc_module_register(struct objc_module* );

///////////////////////////////////////////////////////////////////////////////

void __objc_exec_class(struct objc_module *module) {
    __objc_class_init();
    __objc_hash_init();
    __objc_statics_init();
    __objc_module_register(module);
}

static void __objc_module_register(struct objc_module* module) {
    if (module == NULL || module->version != OBJC_ABI_VERSION) {
        panicf("Invalid abi version: %lu", module ? module->version : 0);
        return;
    }

    // Replace referenced selectors from names to SEL's
    struct objc_selector* refs = module->symtab->refs; 
    if (refs != NULL && module->symtab->sel_ref_cnt > 0) {
        printf("TODO: Replace selectors @%p (sel_ref_cnt=%ld)\n", refs, module->symtab->sel_ref_cnt);
        // TODO: Implement actual selector replacement
        // This should iterate through refs and replace sel_id strings with unique SEL pointers
    }

    unsigned short j = 0;
    for (unsigned short i = 0; i < module->symtab->cls_def_cnt; i++) {
        objc_class_t *cls = (objc_class_t *)module->symtab->defs[j++];
        __objc_class_register(cls);
    }
    for (unsigned short i = 0; i < module->symtab->cat_def_cnt; i++) {
        struct objc_category *cat = (struct objc_category *)module->symtab->defs[j++];
        printf("TODO: Register category %s @%p\n", cat->name, cat);
        __objc_class_category_register(cat);
    }
    
    // Defer processing of static instances
	struct objc_static_instances_list** statics = (void*)module->symtab->defs[j];
    while (statics != NULL && *statics  != NULL) {
        __objc_statics_register(*(statics++));
    }
}
