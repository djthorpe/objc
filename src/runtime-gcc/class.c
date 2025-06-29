#include <stdio.h>
#include <string.h>

#include <objc/objc.h>
#include "api.h"
#include "class.h"
#include "hash.h"

#define CLASS_TABLE_SIZE 32
objc_class_t *class_table[CLASS_TABLE_SIZE+1];

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
    printf("__objc_class_register %c[%s] @%p size=%zu\n", p->info & objc_class_flag_meta ? '+' : '-', p->name, p, p->size);
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
        printf("      %c[%s %s] types=%s imp=%p\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, method->name, method->types, method->imp);
        struct objc_hashitem* item = __objc_hash_register(cls, method->name, method->types, method->imp);
        if (item == NULL) {
            printf("TODO: Failed to register method %s in class %s\n", method->name, cls->name);
        }
    }
}


/*
 * Resolve methods in the class for lookup objc_msg_lookup and objc_msg_lookup_super.
 */
void __objc_class_resolve(objc_class_t *p) {
    printf("  __objc_class_resolve %c[%s] @%p size=%zu\n", p->info & objc_class_flag_meta ? '+' : '-', p->name, p, p->size);

    // Enumerate the class's methods and resolve them
    for (struct objc_method_list *ml = p->methods; ml != NULL; ml = ml->next) {
        __objc_method_list_register_class(p,ml);
    }
}

void __objc_class_category_register(struct objc_category *cat) {
    if (cat == NULL || cat->name == NULL || cat->class_name == NULL) {
        return;
    }
    printf("__objc_class_category_register +[%s+%s]\n", cat->name, cat->class_name);
}

///////////////////////////////////////////////////////////////////////////////

Class objc_lookup_class(const char *name) {
    objc_class_t *cls = __objc_lookup_class(name);
    if (cls == Nil) {
        return Nil;
    }
    printf("objc_lookup_class %c[%s] @%p\n",cls->info & objc_class_flag_meta ? '+' : '-', name, cls);

    // Resolve the class
    if (!(cls->info & objc_class_flag_resolved)) {
        __objc_class_resolve(cls);
        cls->info |= objc_class_flag_resolved; // Mark as resolved
    } else {
        return (Class)cls; // Already resolved
    }
    
    // Resolve the metaclass
    if (cls->metaclass != NULL) {
        __objc_class_resolve(cls->metaclass);
        cls->metaclass->info |= objc_class_flag_resolved; // Mark as resolved
    }

    // Resolve the superclass
    if (cls->superclass != NULL) {
        printf("objc_lookup_class super=\"%s\" @%p\n",(const char* )cls->superclass,cls->superclass);
        Class superclass = __objc_lookup_class((const char* )cls->superclass);
        if (superclass == Nil) {
            panicf("Superclass %s not found for class %s", cls->superclass, cls->name);
            return Nil;
        }
        __objc_class_resolve(superclass);
        cls->superclass = superclass; // Update the superclass pointer
        superclass->info |= objc_class_flag_resolved; // Mark as resolved
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

IMP objc_msg_lookup(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }

    // Get the class of the receiver
    objc_class_t* cls = receiver->isa;
    if (cls == Nil) {
        panicf("objc_msg_lookup: receiver has no class");
        return NULL;
    }

    // Get the implementation pointer for the method
    struct objc_hashitem* item = __objc_hash_lookup(cls, selector->sel_id, selector->sel_type);
    if (item == NULL) {
        panicf("objc_msg_lookup: class=%c[%s %s] selector->types=%s not found\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, (const char* )selector->sel_id, selector->sel_type);
        return NULL; // Method not found
    } else {
        printf("objc_msg_lookup: method=%c[%s %s] => @%p\n", cls->info & objc_class_flag_meta ? '+' : '-', cls->name, (const char *)selector->sel_id, item->imp);
        return item->imp; // Return the implementation pointer
    }
}

IMP objc_msg_lookup_super(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }
    printf("objc_msg_lookup_super: receiver=%p selector->id=%s selector->types=%s\n", receiver, (const char* )selector->sel_id, selector->sel_type);
    return NULL;
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
