#pragma once

@interface NXConstantString : Object {
    const char* data;
    unsigned int length;
}

+(void) test;
-(const char*) cStr;
-(unsigned int) length;

@end
