#pragma once

// Class and Function attributes
#define NS_ROOT_CLASS __attribute__((objc_root_class))

@protocol Object
+ (id) alloc;
- (id) init;
@end

NS_ROOT_CLASS
@interface Object <Object> {
@private
    Class isa;
}
+ (id) alloc;
- (id) init;
@end
