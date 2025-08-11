#pragma once
#include <objc/objc.h>

@interface Test : Object {
    int _value; // Instance variable to hold the value
}

// Lifecycle
-(id) initWithValue:(int)value;

// Properties
-(int)value;
-(void)setValue:(int)value;

@end
