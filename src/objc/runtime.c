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
 * Load a class into the runtime.
 * 
 * @param cls The class to load.
 * @return The class or nil if not found.
 * 
 */
void objc_class_load(objc_class* cls) {
    debugin("objc_class_load");
    if (cls == NULL || objc_class_get(cls->rodata->name)) {
        return;
    }

    debugf("name: %s\n", cls->rodata->name);
    if (cls->rodata->flags & CLASS_RO_META) {
        debugf("%s is a meta class\n", cls->rodata->name);
    }
    if (cls->rodata->flags & CLASS_RO_ROOT) {
        debugf("%s is a root class\n", cls->rodata->name);
    }

    // Load the super class
    if (cls->superclass) {
        debugf("%s has a superclass\n", cls->rodata->name);
        objc_class_load(cls->superclass);
    }

    // Load the methods
    for (uint32_t i = 0; i < cls->rodata->methods->method_count; i++) {
            struct objc_method* method = &cls->rodata->methods->methods[i];            
            debugf("  Method %u address: %p\n", i, (void*)method);   
            debugf("    name: [%c%s %s]\n",cls->rodata->flags & CLASS_RO_META ? '+' : '-', cls->rodata->name,method->name); 
            debugf("    types: %s\n", method->types);
            debugf("    imp: %p\n", (void*)method->imp);        
    }

    // Load the protocols
    for (uint64_t i = 0; i < cls->rodata->protocols->count; i++) {
        struct objc_protocol* protocol = cls->rodata->protocols->protocols[i];
        debugf("  %s protocol: <%s> ", cls->rodata->name, protocol->name);
        debugf("\n");
    }        

    // Load the instance variables
    if (cls->rodata->ivars && cls->rodata->ivars->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->ivars->count; i++) {
            struct objc_ivar* ivar = &cls->rodata->ivars->ivars[i];
            debugf("  %s ivar: <%s>\n", cls->rodata->name, ivar->name);
        }        
    }

    // Load the properties
    if (cls->rodata->properties && cls->rodata->properties->count > 0) {
        for (uint64_t i = 0; i < cls->rodata->properties->count; i++) {
            struct objc_property* property = &cls->rodata->properties->properties[i];
            debugf("  %s property: <%s>\n", cls->rodata->name, property->name);
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

    debugout("objc_class_load");
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Protocols

////////////////////////////////////////////////////////////////////////////////
#pragma mark Messages

id objc_msgSend_impl(id receiver, SEL selector) {
    debugin("objc_msgSend_impl");
    debugf("selector=%s\n", selector);
    debugf("receiver=%p\n", receiver);

    Class cls = receiver->isa;
    debugf("class=%p\n", cls);

    // TODO: This is a fudge until we can work out how to load classes earlier    
    if (!(cls->flags & CLASS_LOADED)) {
        objc_class_load(cls);
    }

    // Return nil for now
    debugout("objc_msgSend_impl");
    return nil;
}

////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialisation

__attribute__((constructor)) static void objc_init(void) {
    debugin("objc_init");
    objc_class_init();
    debugout("objc_init");
}
