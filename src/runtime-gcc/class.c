#include <stdio.h>
#include <string.h>

#include <objc/objc.h>
#include "api.h"
#include "class.h"
#include "hash.h"
#include "statics.h"

#define CLASS_TABLE_SIZE 32
objc_class_t *class_table[CLASS_TABLE_SIZE+1];

// Forward declaration of the static lookup function
static IMP __objc_msg_lookup(objc_class_t* cls, SEL selector, id receiver);

///////////////////////////////////////////////////////////////////////////////

void __objc_class_init() {
    static BOOL init;
    if (init) {
        return; // Already initialized
    }
    init = YES;

    for (int i = 0; i <= CLASS_TABLE_SIZE; i++) {
        class_table[i] = NULL;
    }
}

void __objc_class_register(objc_class_t *p) {
    if (p == NULL || p->name == NULL) {
        return;
    }
#ifdef DEBUG
    printf("__objc_class_register %c[%s] @%p size=%lu\n", p->info & objc_class_flag_meta ? '+' : '-', p->name, p, p->size);
#endif
    for(int i = 0; i < CLASS_TABLE_SIZE; i++) {
        if (class_table[i] == p || class_table[i] == NULL) {
            class_table[i] = p;
            return;
        }
        if (strcmp(class_table[i]->name, p->name) == 0) {
            panicf("Duplicate class named: %s", p->name);
        }
    }
    panicf("Class table is full, cannot register class: %s", p->name);
}

objc_class_t* __objc_lookup_class(const char *name) {
    if (name == NULL) {
        return Nil;
    }
    for(int i = 0; i < CLASS_TABLE_SIZE; i++) {
        if (class_table[i] == NULL || class_table[i]->name == NULL) {
            return Nil; // No class found
        }
        if (strcmp(class_table[i]->name, name) == 0) {
            return class_table[i];
        }
    }
    return Nil; // No class found
}

/*
 * Register a method list in the class.
 * This function registers all methods in the method list.
 */
void __objc_method_list_register_class(objc_class_t* cls, struct objc_method_list *ml) {
    if (ml == NULL) {
        return; // Nothing to register
    }
    for (int i = 0; i < ml->count; i++) {
        struct objc_method *method = &ml->methods[i];
        if (method == NULL || method->name == NULL || method->imp == NULL) {
            continue; // Skip invalid methods
        }
#ifdef DEBUG
        printf("    %c[%s %s] types=%s imp=%p\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, method->name, method->types, method->imp);
#endif
        struct objc_hashitem* item = __objc_hash_register(cls, method->name, method->types, method->imp);
        if (item == NULL) {
            printf("TODO: Failed to register method %s in class %s\n", method->name, cls->name);
        }
    }
}

/*
 * Register methods in the class for lookup objc_msg_lookup and objc_msg_lookup_super.
 */
void __objc_class_register_methods(objc_class_t *p) {
#ifdef DEBUG
    printf("  __objc_class_register_methods %c[%s] @%p size=%lu\n", p->info & objc_class_flag_meta ? '+' : '-', p->name, p, p->size);
#endif

    // Enumerate the class's methods and resolve them
    for (struct objc_method_list *ml = p->methods; ml != NULL; ml = ml->next) {
        __objc_method_list_register_class(p, ml);
    }

    // Assume the superclass is not yet resolved
    if (p->superclass != NULL) {        
        Class superclass = __objc_lookup_class((const char* )p->superclass);
        if (superclass == Nil) {
            panicf("Superclass %s not found for class %s", p->superclass, p->name);
            return;
        }
        if (p->info & objc_class_flag_meta) {
            superclass = superclass->metaclass; // Use the metaclass if this is a metaclass
        }
        __objc_class_register_methods(superclass);
        p->superclass = superclass; // Update the superclass pointer
        superclass->info |= objc_class_flag_resolved; // Mark as resolved
    }
}

void __objc_class_category_register(struct objc_category *cat) {
    if (cat == NULL || cat->name == NULL || cat->class_name == NULL) {
        return;
    }
#ifdef DEBUG
    printf("TODO: __objc_class_category_register +[%s+%s]\n", cat->name, cat->class_name);
#endif
}

///////////////////////////////////////////////////////////////////////////////

Class objc_lookup_class(const char *name) {
    // Lookup the class by name
    objc_class_t *cls = __objc_lookup_class(name);
    if (cls == Nil) {
        return Nil;
    }
#ifdef DEBUG
    printf("objc_lookup_class %c[%s] @%p\n",cls->info & objc_class_flag_meta ? '+' : '-', name, cls);
#endif

    // Resolve the class
    if (!(cls->info & objc_class_flag_resolved)) {
        __objc_class_register_methods(cls);
        cls->info |= objc_class_flag_resolved; // Mark as resolved
    } else {
        return (Class)cls; // Already resolved
    }
    
    // Resolve the metaclass
    if (cls->metaclass != NULL) {
        __objc_class_register_methods(cls->metaclass);
        cls->metaclass->info |= objc_class_flag_resolved; // Mark as resolved
    }

    // Return the class pointer
    return (Class)cls;
}

Class objc_get_class(const char* name) {
    Class cls = objc_lookup_class(name);
    if (cls == Nil) {
        panicf("objc_get_class: class %s not found", name);
        return Nil;
    }
    return cls;
}

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

IMP objc_msg_lookup(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }

    // First load the static instances if not already done
    static BOOL statics_init = NO;
    if (statics_init == NO) {
        statics_init = __objc_statics_load();
    }

    // Get the class of the receiver
    objc_class_t* cls = receiver->isa;
    if (cls == Nil) {
        panicf("objc_msg_lookup: receiver is nil or class not found");
        return NULL;
    }
    return __objc_msg_lookup(cls, selector, receiver);
}

IMP objc_msg_lookup_super(struct objc_super *super, SEL selector) {
    if (super == NULL || super->receiver == nil) {
        return NULL;
    }
    return __objc_msg_lookup(super->superclass, selector, super->receiver);
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Looks up the class with the specified name. Returns Nil if the class is not found.
 */
Class objc_lookupClass(const char* name) {
    return name ? objc_lookup_class(name) : Nil;
}

/**
 * Returns the name of the class, or NULL if the class is Nil.
 */
const char *class_getName(Class cls) {
    return cls ? cls->name : NULL;
}

/**
 * Returns the name of the class of the object, or NULL if the object is nil.
 */
const char *object_getClassName(id obj) {
    return obj ? obj->isa->name : NULL;
}

/**
 * Returns the class of the object. Returns Nil if the object is nil.
 */
Class object_getClass (id object) {
    return object ? object->isa : Nil;
}

/**
  * Sets the class of the object to the specified class.
  */
void object_setClass(id object, Class cls) {
    if (object == NULL || cls == NULL) {
        panicf("object_setClass: object or class is NULL");
        return;
    }
    if (cls->info & objc_class_flag_meta) {
        panicf("object_setClass: cannot set class to a metaclass");
        return;
    }
    object->isa = cls; // Set the class of the object
}

/**
 * Returns the size of an instance of the named class, in bytes. Returns 0 if the class is Nil
 */
size_t class_getInstanceSize(Class cls) {
    return cls ? cls->size : 0;
}
