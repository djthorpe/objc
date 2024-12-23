#pragma once

@interface Object {
    Class isa;
}

// Allocates memory for an object
+ (id)alloc;

// Initializes an object
- (id)init;

@end
