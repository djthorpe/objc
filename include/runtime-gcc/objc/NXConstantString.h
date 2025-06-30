#pragma once

@interface NXConstantString : Object {
    const char* data;
    unsigned int length;
}

-(const char*) cStr;
-(unsigned int) length;

@end

#ifdef __clang__
// Compatibility alias for Clang
@compatibility_alias NSString NXConstantString;
#endif