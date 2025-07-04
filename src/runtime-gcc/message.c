#ifdef DEBUG
#include <stdio.h>
#endif
#include <objc/objc.h>

#include "api.h"
#include "category.h"
#include "class.h"
#include "hash.h"
#include "statics.h"

///////////////////////////////////////////////////////////////////////////////

static IMP __objc_msg_lookup(objc_class_t* cls, SEL selector) {
    if (cls == Nil || selector == NULL) {
        return NULL; // Invalid parameters
    }
#ifdef DEBUG    
    printf("__objc_msg_lookup\n");
#endif

    // Descend through the classes looking for the method
    // TODO: Also look at the categories of the class
    while(cls != Nil) {
#ifdef DEBUG    
        printf("  %c[%s %s] types=%s\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, sel_getName(selector), selector->sel_type);
#endif
        struct objc_hashitem* item = __objc_hash_lookup(cls, selector->sel_id, selector->sel_type);
        if (item != NULL) {
            return item->imp; // Return the implementation pointer
        }
        cls = cls->superclass;
    }

    return NULL; // Method not found
}

/**
 * Message dispatch function. Returns the implementation pointer for 
 * the specified selector. Returns nil if the receive is nil, and panics if 
 * the selector is not found.
 */
IMP objc_msg_lookup(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }

    // First load the static instances and categories
    static BOOL init = NO;
    if (init == NO) {
        __objc_statics_load();
        __objc_category_load();
    }

    // Get the class of the receiver
    objc_class_t* cls = receiver->isa;
    if (cls == Nil) {
        panicf("objc_msg_lookup: receiver @%p class is Nil (selector=%s)", receiver, sel_getName(selector));
        return NULL;
    }
    IMP imp = __objc_msg_lookup(cls, selector);
    if (imp == NULL) {
        panicf("objc_msg_lookup: class=%c[%s %s] selector->types=%s cannot send message\n", receiver->isa->info & objc_class_flag_meta ? '+' : '-', receiver->isa->name, sel_getName(selector), selector->sel_type);
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
        panicf("objc_msg_lookup: class=%c[%s %s] selector->types=%s not found\n", super->receiver->isa->info & objc_class_flag_meta ? '+' : '-', super->receiver->isa->name, sel_getName(selector), selector->sel_type);
    }
    return imp;
}

///////////////////////////////////////////////////////////////////////////////

BOOL class_respondsToSelector(Class cls, SEL selector) {
    if (cls == Nil) {
        return NO;
    }
    if (selector == NULL) {
        panicf("class_respondsToSelector: SEL is NULL");
        return NO;
    }
#ifdef DEBUG    
        printf("class_respondsToSelector %c[%s %s] types=%s\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, sel_getName(selector), selector->sel_type);
#endif
    return __objc_msg_lookup(cls, selector) == NULL ? NO : YES; // Check if the class responds to the selector
}

BOOL class_metaclassRespondsToSelector(Class cls, SEL selector) {
    if (cls == Nil) {
        return NULL;
    }
    if (!(cls->info & objc_class_flag_meta)) {
        cls = cls->metaclass; // Use the metaclass for class methods
    }
    return class_respondsToSelector(cls, selector); // Check if the class responds to the selector
}

const char* sel_getName(SEL sel) {
    if (sel == NULL) {
        return NULL;
    }
    return sel->sel_id; // Return the selector name
}
