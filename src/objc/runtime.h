#pragma once

// Method
typedef struct objc_method {
    const char *name;
    void (*imp)(void);  // Function pointer to method implementation
} objc_method;

// Class
typedef struct objc_class {
    const char *name;
    objc_method *method_list;
    int method_count;
    struct objc_class *superclass;
} objc_class;

// Object
typedef struct objc_object {
    objc_class *isa;  // Points to the class
} objc_object;
