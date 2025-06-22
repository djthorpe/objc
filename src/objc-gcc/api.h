#pragma once
#include <stddef.h>

struct objc_selector_t {
  void* id;
  const char* types;
};

struct objc_symtab_t {
    unsigned long selector_count;     // Unused
    SEL selectors;                    // Unused
    unsigned short class_count;       // Class count
    unsigned short category_count;    // Category count
    void* defs[1];               // Definitions of classes, categories, and static instances
};

struct objc_module_t {
  unsigned long version;              // Compiler revision
  unsigned long size;                 // sizeof(struct objc_module_t )
  const char* name;                   // Name of the file where the module was generated
  struct objc_symtab_t* symtab;       // Symbol table
};

struct objc_class_t {     
  struct objc_class_t* metaclass;     // Pointer to the class's meta class.
  struct objc_class*  superclass;     // Pointer to the super class. NULL for class Object.
  const char*  name;                  // Name of the class.
  long version;                       // Unknown. 
  unsigned long info;                 // Bit mask. See class masks defined above.
  unsigned long size;                 // Size in bytes of the class. The sum of the class definition and all super class definitions.
  struct objc_ivar_list* ivars;       // Pointer to a structure that describes the instance variables in the class definition.  NULL indicates no instance variables.
  struct objc_method_list*  methods;  // Linked list of instance methods defined for the class.
  struct sarray* dtable;              // Pointer to instance method dispatch table.
  struct objc_class_t* subclass_list; // Subclasses
  struct objc_class_t* sibling_cls;
  struct objc_protocol_list* protocols; // Protocols conformed to
  void* gc_object_type;
};

struct objc_category_t {
  const char*   name;                /* Name of the category.  Name contained in the () of the  category definition. */
  const char*   class_name;                   /* Name of the class to which the category belongs. */
  struct objc_method_list_t* instance_methods;             /* Linked list of instance methods defined in the category. NULL indicates no instance methods defined. */
  struct objc_method_list_t*   class_methods;                /* Linked list of factory  methods defined in the category.  NULL indicates no class methods defined. */
  struct objc_protocol_list *protocols;	      /* List of Protocols conformed to */
};
