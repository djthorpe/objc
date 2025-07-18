#include <objc/objc.h>
#include <stdlib.h>

#include "api.h"
#include "category.h"
#include "class.h"
#include "hash.h"
#include "protocol.h"
#include "statics.h"

///////////////////////////////////////////////////////////////////////////////

static void __objc_module_register(struct objc_module *module) {
  if (module == NULL || module->version != OBJC_ABI_VERSION) {
    panicf("Invalid abi version: %lu", module ? module->version : 0);
    return;
  }

  // Replace referenced selectors from names to SEL's
  struct objc_selector *refs = module->symtab->refs;
  if (refs != NULL && module->symtab->sel_ref_cnt > 0) {
#ifdef DEBUG
    objc_printf("TODO: Replace selectors @%p (sel_ref_cnt=%ld)\n", refs,
                module->symtab->sel_ref_cnt);
#endif
    // TODO: Implement actual selector replacement
    // This should iterate through refs and replace sel_id strings with unique
    // SEL pointers
  }

#ifdef DEBUG
  objc_printf("__objc_module_register %s cls_def_cnt=%d cat_def_cnt=%d\n",
              module->name, module->symtab->cls_def_cnt,
              module->symtab->cat_def_cnt);
#endif

  // Defer processing of classes
  unsigned short j = 0;
  for (unsigned short i = 0; i < module->symtab->cls_def_cnt; i++) {
    objc_class_t *cls = (objc_class_t *)module->symtab->defs[j++];
    __objc_class_register(cls);
  }

  // Defer processing of categories
  for (unsigned short i = 0; i < module->symtab->cat_def_cnt; i++) {
    struct objc_category *cat =
        (struct objc_category *)module->symtab->defs[j++];
    __objc_category_register(cat);
  }

  // Defer processing of static instances
  struct objc_static_instances_list **statics = (void *)module->symtab->defs[j];
  while (statics != NULL && *statics != NULL) {
    __objc_statics_register(*(statics++));
  }
}

void __objc_exec_class(struct objc_module *module) {
  __objc_class_init();
  __objc_hash_init();
  __objc_statics_init();
  __objc_category_init();
  __objc_protocol_init();
  __objc_module_register(module);
}

void __objc_force_linking(void) {
  extern void __objc_linking(void);
  __objc_linking();
}
