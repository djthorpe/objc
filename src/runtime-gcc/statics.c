#include "api.h"
#include "class.h"
#include <objc/objc.h>
#include <string.h>
#include <runtime-sys/sys.h>

#define STATICS_TABLE_SIZE 32
static struct objc_static_instances_list *statics_table[STATICS_TABLE_SIZE + 1];

///////////////////////////////////////////////////////////////////////////////

void __objc_statics_init() {
  static BOOL init = NO;
  if (init) {
    return; // Already initialized
  }
  init = YES;
  for (int i = 0; i <= STATICS_TABLE_SIZE; i++) {
    statics_table[i] = NULL;
  }
}

void __objc_statics_register(struct objc_static_instances_list *statics) {
  if (statics == NULL || statics->class_name == NULL) {
    return;
  }
#ifdef DEBUG
  sys_printf("__objc_statics_register [%s]\n", statics->class_name);
#endif
  for (int i = 0; i < STATICS_TABLE_SIZE; i++) {
    if (statics_table[i] == statics) {
      // Static list is already registered, nothing to do
      return;
    }
    if (statics_table[i] == NULL) {
      // Found empty slot, register the static list
      statics_table[i] = statics;
      return;
    }
  }
  sys_panicf("Static instances table is full, cannot register class: %s",
             statics->class_name);
}

static void __objc_statics_load_list(struct objc_static_instances_list *list) {
  // Lookup the class by name - this will resolve the class if it exists
  objc_class_t *cls = objc_lookup_class(list->class_name);
  if (cls == NULL) {
    sys_panicf("Static instances class '%s' not found", list->class_name);
    return;
  }

  // Register each static instance
  for (id *instance = list->instances; *instance != nil; instance++) {
    (*instance)->isa = cls;
  }
}

BOOL __objc_statics_load() {
  static BOOL init = NO;
  if (init) {
    return NO; // Already initialized
  }
  init = YES;

  // Replace class name with resolved class
  for (int i = 0; i < STATICS_TABLE_SIZE; i++) {
    struct objc_static_instances_list *list = statics_table[i];
    if (list == NULL) {
      continue; // No static instances in this slot
    }
    __objc_statics_load_list(list);
  }

  return YES;
}
