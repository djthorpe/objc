#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <objc/runtime.h>
#include "runtime.h"
#include "debug.h"

uint8_t _objc_empty_cache = 0;

// Forward declarations
static bool validate_method_list(struct objc_method_list* list);
static void inspect_sel(const char* name, SEL sel);

////////////////////////////////////////////////////////////////////////////////
#pragma mark Classes

#define MAX_CLASSES 100

// Flags
#define CLASS_RO_META           (1U << 0) // rodata->flags: A meta class
#define CLASS_RO_ROOT           (1U << 1) // rodata->flags: A root class
#define CLASS_LOADED            (1U << 1) // flags: The class has been loaded

// Class table
static objc_class* class_table[MAX_CLASSES];

/** 
 * Initialise the class table.
 * 
 */
void objc_class_init(void) {
    for (int i = 0; i < MAX_CLASSES; i++) {
        class_table[i] = NULL;
    }
}

/** 
 * Get a class by name.
 * 
 * This function searches the class table for a class with the given name.
 * TODO: It's a very simple implementation and not efficient.
 * 
 * @param name The name of the class.
 * @return The class or nil if not found.
 * 
 */
objc_class* objc_class_get(const char *name) {
    for (int i = 0; i < MAX_CLASSES; i++) {
        if (class_table[i] && strcmp(class_table[i]->rodata->name, name) == 0) {
            return class_table[i];
        }
    }
    return NULL;
}

/**
 * Utility function to safely inspect a SEL value
 * 
 * @param name The function name where this is called
 * @param sel The SEL to inspect
 */
static void inspect_sel(const char* name, SEL sel) {
    printf("SEL inspection from %s:\n", name);
    
    if (sel == NULL) {
        printf("  SEL is NULL\n");
        return;
    }
    
    // Print the string value
    printf("  SEL as string: '%s'\n", sel);
    
    // Print the pointer value
    printf("  SEL as pointer: %p\n", (void*)sel);
    
    // Print the first few bytes to see if it's a valid string
    printf("  First 16 bytes: ");
    const unsigned char* bytes = (const unsigned char*)sel;
    for (int i = 0; i < 16 && bytes[i] != '\0'; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
    
    // Check if it's a valid string by seeing if it has a null terminator in a reasonable range
    int has_null = 0;
    for (int i = 0; i < 100; i++) {
        if (bytes[i] == '\0') {
            has_null = 1;
            printf("  Found null terminator at offset %d\n", i);
            break;
        }
    }
    
    if (!has_null) {
        printf("  WARNING: No null terminator found in first 100 bytes!\n");
    }
}

/**
 * Validate method list structure to ensure it's properly formatted
 * 
 * @param list The method list to validate
 * @return true if the list is valid, false otherwise
 */
static bool validate_method_list(struct objc_method_list* list) {
    if (list == NULL) {
        printf("Method list is NULL\n");
        return false;
    }
    
    printf("Validating method list at %p\n", (void*)list);
    printf("  method_size: %u\n", list->method_size);
    printf("  method_count: %u\n", list->method_count);
    
    // Basic sanity checks
    if (list->method_count > 1000) {
        printf("  WARNING: method_count seems too large: %u\n", list->method_count);
        return false;
    }
    
    if (list->method_size < sizeof(struct objc_method)) {
        printf("  WARNING: method_size is too small: %u (expected at least %zu)\n", 
               list->method_size, sizeof(struct objc_method));
        return false;
    }
    
    // Validate first method if count > 0
    if (list->method_count > 0) {
        struct objc_method* first_method = &list->methods[0];
        printf("  First method at address: %p\n", (void*)first_method);
        hexdump(first_method, sizeof(struct objc_method));
    }
    
    return true;
}

/** 
 * Load a class into the runtime.
 * 
 * @param cls The class to load.
 * @return The class or nil if not found.
 * 
 */
void objc_class_load(objc_class* cls) {
    if (cls == NULL || objc_class_get(cls->rodata->name)) {
        return;
    }
    printf("objc_class_load: %s\n", cls->rodata->name);
    if (cls->rodata->flags & CLASS_RO_META) {
        printf("  %s is a meta class\n", cls->rodata->name);
    }
    if (cls->rodata->flags & CLASS_RO_ROOT) {
        printf("  %s is a root class\n", cls->rodata->name);
    }

    // Load the super class
    if (cls->superclass) {
        printf("  %s superclass => \n", cls->rodata->name);
        objc_class_load(cls->superclass);
        printf("  %s superclass <= \n", cls->rodata->name);
    }

    // Load the methods
    printf("  methods =>\n");
    for (uint32_t i = 0; i < cls->rodata->methods->method_count; i++) {
            // Correctly calculate the method pointer from the base of the methods array
            struct objc_method* method = &cls->rodata->methods->methods[i];
            
            // Print method details
            printf("  Method %u address: %p\n", i, (void*)method);
            hexdump(method, sizeof(struct objc_method));
        }
    printf("  <= methods\n");

    // Load the protocols
    if (cls->rodata->protocols && cls->rodata->protocols->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->protocols->count; i++) {
            struct objc_protocol* protocol = cls->rodata->protocols->protocols[i];
            printf("  %s protocol: <%s> ", cls->rodata->name, protocol->name);
            printf("\n");
        }        
    }

    // Load the instance variables
    if (cls->rodata->ivars && cls->rodata->ivars->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->ivars->count; i++) {
            struct objc_ivar* ivar = &cls->rodata->ivars->ivars[i];
            printf("  %s ivar: <%s>\n", cls->rodata->name, ivar->name);
        }        
    }

    // Load the properties
    if (cls->rodata->properties && cls->rodata->properties->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->properties->count; i++) {
            struct objc_property* property = &cls->rodata->properties->properties[i];
            printf("  %s property: <%s>\n", cls->rodata->name, property->name);
        }        
    }

    // TODO: Call +load, if necessary.
    //const Class metaclass = cls->isa;
    //const SEL load = @selector(load);
    //const IMP loadIMP = class_lookupMethodIfPresent(metaclass, load);
    //if (loadIMP) {
    //    ((void (*)(Class, SEL))loadIMP)(cls, load);
    //}

    // Set loaded flag
    cls->flags |= CLASS_LOADED;
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Protocols

////////////////////////////////////////////////////////////////////////////////
#pragma mark Messages

id objc_msgSend_impl(id receiver, SEL selector) {
    Class cls = receiver->isa;

    printf("=> objc_msgSend selector %s\n", selector);

    // TODO: This is a fudge until we can work out how to load classes earlier    
    if (!(cls->flags & CLASS_LOADED)) {
        objc_class_load(cls);
    }

    printf("<= objc_msgSend selector %s\n", selector);

    // Return nil for now
    return nil;
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialisation

__attribute__((constructor)) static void objc_init(void) {
    printf("objc: objc_init\n");
    objc_class_init();
}
