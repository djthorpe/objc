#pragma once
#include <stddef.h>

struct objc_selector_t {
  void* id;                      // Unique identifier for the selector
  const char* types;             // Type encoding for the selector
};

struct objc_symtab_t {
    unsigned long selector_count; // Total number of selectors in this module
    SEL selectors;                // Array of selectors used in this module
    unsigned short class_count;   // Number of classes defined in this module
    unsigned short category_count;// Number of categories defined in this module
    void* defs[1];                // Definitions of classes, categories, and static object instances
};

struct objc_module_t {
  unsigned long version;          // Compiler version used to generate this module
  unsigned long size;             // Size of this structure in bytes
  const char* name;               // Name of the file where this module was generated
  struct objc_symtab_t* symtab;   // Pointer to the symbol table for this module
};

struct objc_class_t {     
  struct objc_class_t* metaclass; // Pointer to the metaclass for this class
  struct objc_class_t* superclass; // Pointer to the superclass. NULL for the root class
  const char* name;               // Name of the class
  long version;                   // Version of the class (unused)
  unsigned long info;             // Bitmask containing class-specific objc_class_flags
  unsigned long size;             // Total size of the class, including all superclasses
  struct objc_ivar_list_t* ivars; // List of instance variables defined in this class
  struct objc_method_list_t* methods; // List of instance methods defined in this class
  struct sarray* dtable;          // Dispatch table for instance methods
  struct objc_class_t* subclass_list; // Pointer to the first subclass of this class
  struct objc_class_t* sibling_cls; // Pointer to sibling classes
  struct objc_protocol_list* protocols; // List of protocols adopted by this class
  void* extra_data;               // Additional data associated with this class
};

enum objc_class_flags {
  objc_class_flag_meta = 0x02,        // This class structure represents a metaclass
  objc_class_flag_initialized = 0x04, // Indicates the class has received the +initialize message
  objc_class_flag_resolved = 0x08,    // Indicates the class has been initialized by the runtime
};

struct objc_category_t {
  const char* name;               // Name of the category
  const char* class_name;         // Name of the class to which this category belongs
  struct objc_method_list_t* instance_methods; // List of instance methods defined in this category
  struct objc_method_list_t* class_methods; // List of class methods defined in this category
  struct objc_protocol_list* protocols; // List of protocols adopted by this category
};

struct objc_method_t {
  SEL selector;                   // Selector for this method
  const char* types;              // Type encoding for this method
  IMP imp;                        // Pointer to the function implementing this method
};

struct objc_ivar_t {
  const char* name;               // Name of the instance variable
  const char* type;               // Type encoding for the instance variable
  int offset;                     // Offset of the instance variable from the start of the object
};

struct objc_method_list_t {
  struct objc_method_list_t* next; // Pointer to the next method list in the chain
  int count;                       // Number of methods in this list
  size_t size;                     // Size of the method list structure
  struct objc_method_t methods[1]; // Array of methods in this list
};

struct objc_ivar_list_t {
  int count;                      // Number of instance variables in this list
  struct objc_ivar_t ivars[1];    // Array of instance variable metadata
};
