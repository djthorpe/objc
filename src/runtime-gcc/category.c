#include "category.h"
#include "api.h"
#include "class.h"
#include <objc/objc.h>
#include <runtime-sys/sys.h>

#define CATEGORY_TABLE_SIZE 32
static struct objc_category *category_table[CATEGORY_TABLE_SIZE + 1];

///////////////////////////////////////////////////////////////////////////////

void __objc_category_init() {
  static BOOL init = NO;
  if (init) {
    return; // Already initialized
  }
  init = YES;
  for (int i = 0; i <= CATEGORY_TABLE_SIZE; i++) {
    category_table[i] = NULL;
  }
}

void __objc_category_register(struct objc_category *category) {
  if (category == NULL || category->name == NULL ||
      category->class_name == NULL) {
    return;
  }
#ifdef DEBUG
  sys_printf("__objc_category_register [%s+%s]\n", category->class_name,
             category->name);
#endif
  for (int i = 0; i < CATEGORY_TABLE_SIZE; i++) {
    if (category_table[i] == category) {
      // Category is already registered, nothing to do
      return;
    }
    if (category_table[i] == NULL) {
      // Found empty slot, register the category
      category_table[i] = category;
      return;
    }
  }
  sys_panicf("Category table is full, cannot register category: %s",
             category->name);
}

static void __objc_category_load_category(struct objc_category *category) {
  // Lookup the class by name
  objc_class_t *cls = objc_lookup_class(category->class_name);
  if (cls == Nil) {
    return;
  }

#ifdef DEBUG
  sys_printf("  __objc_category_load_category [%s+%s]\n", cls->name,
             category->name);
#endif

  // Register instance methods from the category
  if (category->instance_methods != NULL) {
    for (struct objc_method_list *ml = category->instance_methods; ml != NULL;
         ml = ml->next) {
      __objc_class_register_method_list(cls, ml);
    }
  }

  // Register class methods from the category
  if (category->class_methods != NULL && cls->metaclass != NULL) {
    for (struct objc_method_list *ml = category->class_methods; ml != NULL;
         ml = ml->next) {
      __objc_class_register_method_list(cls->metaclass, ml);
    }
  }
}

BOOL __objc_category_load() {
  static BOOL init = NO;
  if (init) {
    return NO; // Already loaded
  }
  init = YES;

  // Replace class name with resolved class
  for (int i = 0; i < CATEGORY_TABLE_SIZE; i++) {
    struct objc_category *category = category_table[i];
    if (category == NULL) {
      continue; // Skip empty slots and continue searching
    }
    __objc_category_load_category(category);
  }

  return YES;
}
