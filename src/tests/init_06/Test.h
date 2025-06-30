#pragma once
#include <objc/objc.h>

// Class
@interface Test : Object {
    NXConstantString* _value;
}

// Lifecycle
-(id) initWithString:(NXConstantString* )value;

@end
