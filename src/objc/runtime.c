#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <objc/runtime.h>
#include "runtime.h"

uint8_t _objc_empty_cache = 0;

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
 * It's a very simple implementation and not efficient.
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

    // Load the super class
    if (cls->superclass) {
        objc_class_load(cls->superclass);
    }

    // Load the methods
    if(cls->rodata->methods && cls->rodata->methods->element_count > 0) {
        for (uint64_t i = 0; i < cls->rodata->methods->element_count; i++) {
            struct objc_method* method = &cls->rodata->methods->methods[i];
            printf("  %s method: -[%s %s]\n", cls->rodata->name,cls->rodata->name, method->name);
        }
    }

    // Load the protocols
    if(cls->rodata->protocols && cls->rodata->protocols->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->protocols->count; i++) {
            struct objc_protocol* protocol = cls->rodata->protocols->protocols[i];
            printf("  %s protocol: <%s> ", cls->rodata->name,protocol->name);
            hexdump(protocol,sizeof(struct objc_protocol));            
            printf("\n");
        }        
    }

    // Load the instance variables
    if(cls->rodata->ivars && cls->rodata->ivars->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->ivars->count; i++) {
            struct objc_ivar* ivar = &cls->rodata->ivars->ivars[i];
            printf("  %s ivar: <%s>\n", cls->rodata->name,ivar->name);
        }        
    }

    // Load the properties
    if(cls->rodata->properties && cls->rodata->properties->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->properties->count; i++) {
            struct objc_property* property = &cls->rodata->properties->properties[i];
            printf("  %s property: <%s>\n", cls->rodata->name,property->name);
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
    //Class cls = receiver->isa;
    printf("objc_msgSend selector %s\n", selector);

    // Return nil for now
    return nil;
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialisation

__attribute__((constructor)) static void objc_init(void) {
    printf("objc: objc_init\n");
    objc_class_init();
}
