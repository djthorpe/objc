#pragma once

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
