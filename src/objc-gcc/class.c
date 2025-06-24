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

void __objc_class_register(struct objc_class_t *p) {
    printf("__objc_class_register %s\n",p->name);
    if (p == NULL || p->name == NULL) {
        panicf("Invalid class registration: NULL pointer or class name is NULL.");
    }
    for(int i = 0; i < CLASS_TABLE_SIZE; i++) {
        if (class_table[i] == NULL) {
            class_table[i] = p;
            return;
        }
    }
    panicf("Class table is full, cannot register class: %s", p->name);
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
    // Enumerate the class's methods and resolve them
    for (struct objc_method_list_t *ml = p->methods; ml != NULL; ml = ml->next) {
        printf("  __objc_class_resolve %s num_methods=%d\n", p->name, ml->count);
        for (int i = 0; i < ml->count; i++) {
            struct objc_method_t *method = &ml->methods[i];
            printf("    method: %p\n", method);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

Class objc_lookup_class(const char *name) {
    printf("objc_lookup_class %s\n",name);
    struct objc_class_t *cls = __objc_lookup_class(name);
    if (cls == Nil) {
        return Nil;
    }
    // Check to see if the class is resolved
    if (!(cls->info & objc_class_flag_resolved)) {
        // If not resolved, we need to resolve it
        __objc_class_resolve(cls);
        cls->info |= objc_class_flag_resolved; // Mark as resolved
    }
    return (Class)cls;
}

IMP objc_msg_lookup(id receiver, SEL selector) {
    if (receiver == NULL) {
        return NULL;
    }
    printf("objc_msg_lookup: %p %p\n",receiver, selector);
    return NULL;
}
