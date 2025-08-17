#include "api.h"
#include "category.h"
#include "class.h"
#include "hash.h"
#include "statics.h"
#include <objc/objc.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////

// This function is called when a message is sent to a nil object.
static id __objc_nil_method(id receiver, SEL selector OBJC_UNUSED) {
  return receiver;
}

static IMP __objc_msg_lookup(objc_class_t *cls, SEL selector) {
  if (cls == Nil || selector == NULL) {
    return NULL; // Invalid parameters
  }
#ifdef OBJCDEBUG
  sys_printf("objc_msg_lookup %c[%s %s]\n",
             cls->info & objc_class_flag_meta ? '+' : '-', cls->name,
             sel_getName(selector));
#endif

  // Descend through the classes looking for the method
  // TODO: Also look at the categories of the class
  while (cls != Nil) {
#ifdef OBJCDEBUG
    sys_printf("  %c[%s %s] types=%s\n",
               cls->info & objc_class_flag_meta ? '+' : '-', cls->name,
               sel_getName(selector), selector->sel_type);
#endif
    struct objc_hashitem *item =
        __objc_hash_lookup(cls, selector->sel_id, selector->sel_type);
    if (item != NULL) {
      return item->imp; // Return the implementation pointer
    }
    cls = cls->superclass;
  }

  return NULL; // Method not found
}

static void __objc_send_initialize(objc_class_t *cls) {
  if (cls == Nil) {
    return;
  }

  // Don't call initialize on the same class twice
  if (cls->info & objc_class_flag_initialized) {
    return;
  }

  // Mark the class as initialized early to prevent recursion
  cls->info |= objc_class_flag_initialized;

  // If the superclass has an initialize method, call it first
  if (cls->superclass) {
    __objc_send_initialize(cls->superclass);
  }

  // Find and call the initialize method
  static struct objc_selector initialize = {
      .sel_id = "initialize", // The selector for the initialize method
      .sel_type = "v16@0:8"   // The type encoding for the initialize method
  };
  IMP imp = __objc_msg_lookup(cls, &initialize); // Lookup the initialize method
  if (imp != NULL) {
    // Call the initialize method - suppress function cast warning as this is a
    // legitimate cast from variadic IMP to non-variadic function for
    // +initialize which takes no parameters
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
    ((void (*)(id, SEL))imp)(
        (id)cls, &initialize); // Call the initialize method on the class
#pragma GCC diagnostic pop
  }
}

/**
 * Message dispatch function. Returns the implementation pointer for
 * the specified selector. Returns the nil_method if the receiver is nil,
 * and panics if the selector is not found.
 */
IMP objc_msg_lookup(id receiver, SEL selector) {
  if (receiver == NULL) {
    return (IMP)__objc_nil_method;
  }

  // First load the static instances and categories
  static BOOL init = NO;
  if (init == NO) {
    init = YES; // Set init to YES to prevent multiple initializations
    __objc_statics_load();
    __objc_category_load();
  }

  // Get the class of the receiver
  objc_class_t *cls = receiver->isa;
  if (cls == Nil) {
    sys_panicf("objc_msg_lookup: receiver @%p class is Nil (selector=%s)",
               receiver, sel_getName(selector));
    return NULL;
  }

  IMP imp = __objc_msg_lookup(cls, selector);
  if (imp == NULL) {
    sys_panicf(
        "objc_msg_lookup: class=%c[%s %s] selector->types=%s cannot send "
        "message\n",
        receiver->isa->info & objc_class_flag_meta ? '+' : '-',
        receiver->isa->name, sel_getName(selector), selector->sel_type);
  } else {
#ifdef OBJCDEBUG
    sys_printf("    => IMP @%p\n", imp);
#endif
  }

  // If the class has of the receiver not been initialized, then this is the
  // time to do it
  objc_class_t *meta_cls =
      cls->info & objc_class_flag_meta ? cls : cls->metaclass;
  if (!(meta_cls->info & objc_class_flag_initialized)) {
#ifdef OBJCDEBUG
    sys_printf("  +[%s initialize] \n", cls->name);
#endif
    // Call the class's initialize method
    __objc_send_initialize(meta_cls);
  }

  return imp;
}

/**
 * Message superclass dispatch function. Returns the implementation pointer for
 * the specified selector, for the receiver superclass. Returns nil if the
 * receiver is nil.
 */
IMP objc_msg_lookup_super(struct objc_super *super, SEL selector) {
  if (super == NULL || super->receiver == nil) {
    return NULL;
  }
  IMP imp = __objc_msg_lookup(super->superclass, selector);
  if (imp == NULL) {
    sys_panicf(
        "objc_msg_lookup: class=%c[%s %s] selector->types=%s not found\n",
        super->receiver->isa->info & objc_class_flag_meta ? '+' : '-',
        super->receiver->isa->name, sel_getName(selector), selector->sel_type);
  }
  return imp;
}

///////////////////////////////////////////////////////////////////////////////

BOOL class_respondsToSelector(Class cls, SEL selector) {
  if (cls == Nil) {
    return NO;
  }
  if (selector == NULL) {
    sys_panicf("class_respondsToSelector: SEL is NULL");
    return NO;
  }
#ifdef OBJCDEBUG
  sys_printf("class_respondsToSelector %c[%s %s] types=%s\n",
             cls->info & objc_class_flag_meta ? '+' : '-', cls->name,
             sel_getName(selector), selector->sel_type);
#endif
  return __objc_msg_lookup(cls, selector) == NULL
             ? NO
             : YES; // Check if the class responds to the selector
}

BOOL object_respondsToSelector(id object, SEL selector) {
  if (object == NULL) {
    return NO; // If the object is nil, it cannot respond to any selector
  }
  if (selector == NULL) {
    sys_panicf("object_respondsToSelector: SEL is NULL");
    return NO;
  }
  Class cls = object_getClass(object);
#ifdef OBJCDEBUG
  sys_printf("object_respondsToSelector %c[%s %s] types=%s\n",
             cls->info & objc_class_flag_meta ? '+' : '-', cls->name,
             sel_getName(selector), selector->sel_type);
#endif
  return __objc_msg_lookup(cls, selector) == NULL ? NO : YES;
}

BOOL class_metaclassRespondsToSelector(Class cls, SEL selector) {
  if (cls == Nil) {
    return NO;
  }
  if (!(cls->info & objc_class_flag_meta)) {
    cls = cls->metaclass; // Use the metaclass for class methods
  }
  return class_respondsToSelector(
      cls, selector); // Check if the class responds to the selector
}

const char *sel_getName(SEL sel) {
  if (sel == NULL) {
    return NULL;
  }
  return sel->sel_id; // Return the selector name
}
