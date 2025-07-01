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

static IMP __objc_msg_lookup(objc_class_t* cls, SEL selector, id receiver) {
    if (cls == Nil || selector == NULL || receiver == NULL) {
        return NULL; // Invalid parameters
    }
#ifdef DEBUG    
    printf("objc_msg_lookup: %c[%s %s]\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, (const char* )selector->sel_id);
#endif

    // Descend through the classes looking for the method
    // TODO: Also look at the categories of the class
    while(cls != Nil) {
#ifdef DEBUG    
        printf("  %c[%s %s:%s]\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, (const char* )selector->sel_id, selector->sel_type);
#endif
        struct objc_hashitem* item = __objc_hash_lookup(cls, selector->sel_id, selector->sel_type);
        if (item != NULL) {
            return item->imp; // Return the implementation pointer
        }
        cls = cls->superclass;
    }

    panicf("objc_msg_lookup: class=%c[%s %s] selector->types=%s not found\n", receiver->isa->info & objc_class_flag_meta ? '+' : '-', receiver->isa->name, (const char* )selector->sel_id, selector->sel_type);
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
        panicf("objc_msg_lookup: receiver is nil or class not found");
        return NULL;
    }
    return __objc_msg_lookup(cls, selector, receiver);
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
    return __objc_msg_lookup(super->superclass, selector, super->receiver);
}
