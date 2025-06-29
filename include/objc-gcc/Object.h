#pragma once
#include "runtime.h"

OBJC_ROOT_CLASS
@interface Object {
    Class isa;
}

-(Class) class;
-(BOOL) isEqual:(id)anObject;
+(id) alloc;
-(void) free;

@end
