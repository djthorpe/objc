#pragma once
#include <objc/objc.h>
#include "Test2.h"

@interface Test2 : Object {
    int _value2; // Instance variable to hold the value
}

// Lifecycle
-(id) initWithValue:(int)value;

// Properties
-(int)value;
-(void)setValue:(int)value;
-(int) value2;
-(void) setValue2:(int)value;

@end
