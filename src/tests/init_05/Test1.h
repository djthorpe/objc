#pragma once
#include <objc/objc.h>
#include "Test2.h"

@interface Test1 : Test2 {
    int _value1; // Instance variable to hold the value
}

// Lifecycle
-(id) initWithValue:(int)value;

// Properties
-(int) value;
-(void) setValue:(int)value;
-(int) value1;
-(void) setValue1:(int)value;


@end
