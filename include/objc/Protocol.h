
@interface Protocol : Object {
    const char* name;
    struct objc_protocol_list* protocols;
    struct objc_method_list* instance_methods;
    struct objc_method_list* class_methods;
    struct objc_method_list *optional_instance_methods;
    struct objc_method_list *optional_class_methods;	  
    struct objc_property_list* properties;	  
    struct objc_property_list* optional_properties;	
}

@end
