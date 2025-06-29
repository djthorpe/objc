#include <stdio.h>
#include <string.h>

#include <objc/objc.h>
#include "hash.h"

#define HASH_TABLE_SIZE 512
struct objc_hashitem hash_table[HASH_TABLE_SIZE+1];

static size_t __objc_hash_compute(objc_class_t* cls, const char* method, const char* types);

///////////////////////////////////////////////////////////////////////////////

void __objc_hash_init() {
    static BOOL init;
    if (init) {
        return; // Already initialized
    }
    init = YES;

    // Initialize the hash table
    for (int i = 0; i <= HASH_TABLE_SIZE; i++) {
        hash_table[i].cls = NULL;
    }
}

/*
 * Register a new method in the hash table
 */
struct objc_hashitem* __objc_hash_register(objc_class_t* cls, const char* method, const char* types, IMP imp) {
    size_t hash = __objc_hash_compute(cls, method, types);
    size_t index = hash;
    while (hash_table[hash].cls != NULL) {
        // Collision detected, find the next available slot
        hash = (hash + 1) % HASH_TABLE_SIZE;
        if (hash == index) {
            return NULL;
        }
    }

    // Insert the new hash item
    struct objc_hashitem* item = &hash_table[hash];
    item->cls = cls;
    item->method = method;
    item->types = types;
    item->imp = imp;

    // Return the newly registered item
    return item;
}


/*
 * Lookup an implementation for a method in the hash table
 */
struct objc_hashitem* __objc_hash_lookup(objc_class_t* cls, const char* method, const char* types) {
    size_t hash = __objc_hash_compute(cls, method, types);
    size_t index = hash;
    while (hash_table[hash].cls != NULL) {
        // Check if the class, method, and types match
        if (hash_table[hash].cls == cls &&
            hash_table[hash].method != NULL &&
            strcmp(hash_table[hash].method, method) == 0 &&
            hash_table[hash].types != NULL &&
            strcmp(hash_table[hash].types, types) == 0) {
            break;
        }
        // Increment the hash to check the next item
        hash = (hash + 1) % HASH_TABLE_SIZE;
        if (hash == index) {
            return NULL;
        }
    }

    // Return the hash item
    return hash_table[hash].cls == NULL ? NULL : &hash_table[hash];
}

static size_t __objc_hash_compute(objc_class_t* cls, const char* method, const char* types) {
    size_t hash = 0;
    for (const char *p = cls->name; *p; p++) {
        hash = (hash * 31) + *p;
    }
    if(cls->info & objc_class_flag_meta) {
        hash += 0x10000; // Meta class flag
    }
    for (const char *p = method; *p; p++) {
        hash = (hash * 31) + *p;
    }
    for (const char *p = types; *p; p++) {
        hash = (hash * 31) + *p;
    }
    return hash % HASH_TABLE_SIZE;
}
