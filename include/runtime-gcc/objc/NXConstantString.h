#pragma once

@interface NXConstantString : Object {
    const char* data;
    unsigned int length;
}

-(const char*) cStr;
-(unsigned int) length;

@end
