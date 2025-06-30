#pragma once

OBJC_ROOT_CLASS
@interface Object {
    Class isa;
}

+(id) alloc;
-(id) init;
-(Class) class;
-(BOOL) isEqual:(id)anObject;
-(void) free;

@end
