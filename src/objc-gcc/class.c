#include <stdio.h>
#include <string.h>
#include <objc-gcc/objc.h>
#include "api.h"

#define CLASS_TABLE_SIZE 1024
struct objc_class_t *class_table[CLASS_TABLE_SIZE+1];

///////////////////////////////////////////////////////////////////////////////

void __objc_class_init() {
    printf("__objc_class_init\n");
    for (int i = 0; i <= CLASS_TABLE_SIZE; i++) {
        class_table[i] = NULL;
    }
}

void __objc_class_register_inner(struct objc_class_t *p) {
    if (p == NULL || p->name == NULL) {
        return;
    }
    for(int i = 0; i < CLASS_TABLE_SIZE; i++) {
        if (class_table[i] == p) {
            return;
        }
        if (class_table[i] == NULL) {
            printf("  __objc_class_register_inner %c[@%p]\n",p->info & objc_class_flag_meta ? '+' : '-',p);
            class_table[i] = p;
            return;
        }
    }
    panicf("Class table is full, cannot register class: %s", p->name);
}

void __objc_class_register(struct objc_class_t *p) {
    if (p == NULL || p->name == NULL) {
        return;
    }
    printf("__objc_class_register %c[%s] @%p\n",p->info & objc_class_flag_meta ? '+' : '-',p->name, p);
    __objc_class_register_inner(p);
    __objc_class_register_inner(p->metaclass);
    __objc_class_register_inner(p->superclass);
}

struct objc_class_t* __objc_lookup_class(const char *name) {
    if (name == NULL) {
        return Nil;
    }
    struct objc_class_t *p = class_table[0];
    while(p != NULL) {
        if (strcmp(p->name, name) == 0) {
            return (Class)p;
        }
        p++;
    }
    return Nil;
}

void __objc_class_resolve(struct objc_class_t *p) {
    printf("  __objc_class_resolve %c[%s]\n", p->info & objc_class_flag_meta ? '+' : '-',p->name);
    // Enumerate the class's methods and resolve them
    for (struct objc_method_list_t *ml = p->methods; ml != NULL; ml = ml->next) {
        for (int i = 0; i < ml->count; i++) {
            struct objc_method_t *method = &ml->methods[i];
            printf("    method: selector=%s types=%s imp=%p\n", method->selector, method->types, method->imp);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

Class objc_lookup_class(const char *name) {
    struct objc_class_t *cls = __objc_lookup_class(name);
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
        printf("  superclass %c[@%p]\n",cls->superclass->info & objc_class_flag_meta ? '+' : '-', cls->superclass);
        //__objc_class_resolve(cls->superclass);
        //cls->superclass->info |= objc_class_flag_resolved; // Mark as resolved
    }
    
    return (Class)cls;
}

IMP objc_msg_lookup(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }
    printf("objc_msg_lookup: receiver=%p selector->id=%s selector->types=%s\n",receiver, selector->id, selector->types);
    return NULL;
}
