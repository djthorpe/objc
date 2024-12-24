#pragma once

#include <objc/objc.h>
#include <stdint.h>
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
#pragma mark Types

// Class
typedef struct objc_class {
    struct objc_class* isa;
    struct objc_class* superclass;
    union {
        uintptr_t flags;
        struct objc_class* nonmeta;
    }; // void *cache;
    struct objc_class_rw* rwdata; // void *vtable;
    struct objc_class_ro* rodata;
} objc_class;

struct objc_class_ro {
    uint32_t flags;
    uint32_t instance_start;
    uint32_t instance_size;
    uint8_t* ivar_layout;
    const char* name;
    struct objc_method_list* methods;
    struct objc_protocol_list* protocols;
    struct objc_ivar_list* ivars;
    uint8_t* weakIvarLayout;
    struct objc_property_list* properties;
    uint32_t reserved;     
};

// Protocol
typedef struct objc_protocol {
    const char* name;
    struct objc_protocol_list* protocols;
    struct objc_method_list* instance_methods;
    struct objc_method_list* class_methods;
    struct objc_method_list *optional_instance_methods;
    struct objc_method_list *optional_class_methods;	  
    struct objc_property_list* properties;	  
    struct objc_property_list* optional_properties;	
} objc_protocol;

struct objc_protocol_list {
    uint64_t count;
    struct objc_protocol *protocols[1];
};

// Method
struct objc_method {
    SEL name;
    char* types;
    IMP imp;
};

struct objc_method_list {
    unsigned int element_size;
    unsigned int element_count;
    struct objc_method methods[];
};

// IVars
typedef struct objc_ivar {
    uint64_t offset;
    const char* name;
    const char* type;
    uint32_t alignment;
    uint32_t size;
} objc_ivar;

struct objc_ivar_list {
    uint32_t size;
    uint32_t count;
    struct objc_ivar ivars[];
};

// Properties
typedef struct objc_property {
    const char* name;
    const char* attributes;
} objc_property;

struct objc_property_list {
    uint32_t size;
    uint32_t count;
    struct objc_property properties[];
};

// Object
typedef struct objc_object {
    objc_class* isa;  // Points to the class
} objc_object;

///////////////////////////////////////////////////////////////////////////////
#pragma mark Function Declarations

// Classes
void objc_class_load(objc_class* cls);
