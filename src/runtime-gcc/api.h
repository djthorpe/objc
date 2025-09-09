#pragma once
#include <objc/objc.h>
#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////////////////
// Objective-C ABI

// This is the current API version for the Objective-C runtime.
#define OBJC_ABI_VERSION 8

struct objc_selector {
  void *sel_id;   // Unique identifier for the selector
  char *sel_type; // Type encoding for the selector
};

struct objc_super {
  id receiver;                   // The receiver of the message
  struct objc_class *superclass; // The superclass of the receiver
};

struct objc_symtab {
  unsigned long sel_ref_cnt;  // Number of selectors referenced in this module
  struct objc_selector *refs; // Array of selectors referenced in this module
  unsigned short cls_def_cnt; // Number of classes defined in this module
  unsigned short cat_def_cnt; // Number of categories defined in this module
  void *defs[1]; // Definitions of classes, categories, and static object
                 // instances
};

struct objc_module {
  unsigned long version; // Compiler version used to generate this module
  unsigned long size;    // Size of this structure in bytes
  const char *name;      // Name of the file where this module was generated
  struct objc_symtab *symtab; // Pointer to the symbol table for this module
};

struct objc_object {
  struct objc_class *isa; // Pointer to the class of this object
};

struct objc_class {
  struct objc_class *metaclass; // Pointer to the metaclass for this class
  struct objc_class
      *superclass;    // Pointer to the superclass. NULL for the root class
  const char *name;   // Name of the class
  long version;       // Version of the class (unused)
  unsigned long info; // Bitmask containing class-specific objc_class_flags
  unsigned long size; // Total size of the class, including all superclasses
  struct objc_ivar_list
      *ivars; // List of instance variables defined in this class
  struct objc_method_list
      *methods;  // List of instance methods defined in this class
  void **dtable; // Dispatch table for instance methods
  struct objc_class
      *subclass_list;             // Pointer to the first subclass of this class
  struct objc_class *sibling_cls; // Pointer to sibling classes
  struct objc_protocol_list
      *protocols;   // List of protocols adopted by this class
  void *extra_data; // Additional data associated with this class
};

enum objc_class_flags {
  objc_class_flag_meta = 0x02, // This class structure represents a metaclass
  objc_class_flag_initialized =
      0x04, // Indicates the class has received the +initialize message
  objc_class_flag_resolved =
      0x08, // Indicates the class has been initialized by the runtime
};

struct objc_category {
  const char *name;       // Name of the category
  const char *class_name; // Name of the class to which this category belongs
  struct objc_method_list
      *instance_methods; // List of instance methods defined in this category
  struct objc_method_list
      *class_methods; // List of class methods defined in this category
  struct objc_protocol_list
      *protocols; // List of protocols adopted by this category
};

struct objc_method {
  union {
    const char *name; // Name of the method (selector)
    SEL selector;     // Selector for this method
  };
  const char *types; // Type encoding for this method
  IMP imp;           // Pointer to the function implementing this method
};

struct objc_ivar {
  const char *name; // Name of the instance variable
  const char *type; // Type encoding for the instance variable
  int offset; // Offset of the instance variable from the start of the object
};

struct objc_method_list {
  struct objc_method_list *next; // Pointer to the next method list in the chain
  int count;                     // Number of methods in this list
  struct objc_method methods[1]; // Array of methods in this list
};

struct objc_ivar_list {
  int count;                 // Number of instance variables in this list
  struct objc_ivar ivars[1]; // Array of instance variable metadata
};

struct objc_static_instances_list {
  const char *class_name;
  id instances[0]; // Flexible array member
};

struct objc_protocol {
  struct objc_class *class;
  const char *name;
  struct objc_protocol_list *protocol_list;
  struct objc_method_list *instance_methods;
  struct objc_method_list *class_methods;
};

struct objc_protocol_list {
  struct objc_protocol_list *next; // Pointer to next protocol list in the chain
  size_t count;                    // Number of protocols in this list
  struct objc_protocol *protocols[1]; // Array of protocols in this list
};
