#ifdef DEBUG
#include <stdio.h>
#endif
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

/**
 * Lookup the class in the class table by name.
 * Returns the class, or Nil if not found.
 */
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

/**
 * Register a list of methods for a class.
 * This function registers all methods in the method list, for the named class.
 */
void __objc_class_register_method_list(objc_class_t* cls, struct objc_method_list *ml) {
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
            panicf("TODO: Failed to register method %s in class %s\n", method->name, cls->name);
            return;
        }
    }
}

/**
 * Register methods in the class for lookup objc_msg_lookup and objc_msg_lookup_super.
 */
void __objc_class_register_methods(objc_class_t *p) {
    // Check if the class is already resolved
    if (p->info & objc_class_flag_resolved) {
        return; // Already resolved
    }

#ifdef DEBUG
    printf("  __objc_class_register_methods %c[%s] @%p size=%lu\n", p->info & objc_class_flag_meta ? '+' : '-', p->name, p, p->size);
#endif

    // Enumerate the class's methods and resolve them
    for (struct objc_method_list *ml = p->methods; ml != NULL; ml = ml->next) {
        __objc_class_register_method_list(p, ml);
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

        // Resolve the metaclass
    if (p->metaclass != NULL) {     
        __objc_class_register_methods(p->metaclass);
        // Set the metaclass superclass
        if (p->superclass != NULL) {
            p->metaclass->superclass = p->superclass->metaclass;
        }
        p->metaclass->info |= objc_class_flag_resolved; // Mark as resolved
    }

}

///////////////////////////////////////////////////////////////////////////////

/**
 * Lookup the class with the specified name, and resolve all the methods for the
 * class and metaclass if that has not yet been done. If the class is not found,
 * Nil is returned.
 */
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
    //if (cls->metaclass != NULL) {        
    //    __objc_class_register_methods(cls->metaclass);
    //    cls->metaclass->info |= objc_class_flag_resolved; // Mark as resolved
    //}

    // Return the class pointer
    return (Class)cls;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Class lookup function. Panics if the class is not found.
 */
Class objc_get_class(const char* name) {
    Class cls = objc_lookup_class(name);
    if (cls == Nil) {
        panicf("objc_get_class: class %s not found", name);
        return Nil;
    }
    return cls;
}

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
 * Checks if an instance class matches, or subclass of another class.
 */
BOOL object_isKindOfClass(id object, Class cls) {
    if (object == nil) {
        return NO;
    }
    if (cls == Nil) {
        panicf("object_isKindOfClass: class is Nil");
        return NO;
    }    
    Class objClass = object->isa; // Get the class of the object
    while (objClass != Nil) {
        if (objClass == cls) {
            return YES; // Found a match
        }
        objClass = objClass->superclass; // Move up the superclass chain
    }
    return NO; // No match found
}

/**
 * Returns the size of an instance of the named class, in bytes. Returns 0 if the class is Nil
 */
size_t class_getInstanceSize(Class cls) {
    return cls ? cls->size : 0;
}

/**
 * Returns the superclass of an instance, or Nil if it is a root class
 */
Class object_getSuperclass(id obj) {
    return obj ? obj->isa->superclass : Nil;
}

/**
 * Returns the superclass of a class, or Nil if it is a root class
 */
Class class_getSuperclass(Class cls) {
    return cls ? cls->superclass : Nil;
}
