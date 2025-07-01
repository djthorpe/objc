#pragma once

@interface NXConstantString : Object {
    const char* _data;
    unsigned int _length;
}

// Lifecycle
+(id) withCString:(const char* )cStr;

// Properties
-(const char* ) cStr;
-(unsigned int) length;

@end

#ifdef __clang__
// Compatibility alias for Clang
@compatibility_alias NSString NXConstantString;
#endif
