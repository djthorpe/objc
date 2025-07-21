#include <objc/objc.h>
#include <string.h>
#include <stdlib.h>

@implementation NXConstantString

#pragma mark - Lifecycle

+ (id)alloc {
  return nil; // NXConstantString should not be allocated directly
}

-(void)dealloc {
  // NXConstantString is immutable, so we do nothing
}

#pragma mark - Properties

- (const char *)cStr {
  return _data;
}

- (unsigned int)length {
  return _length;
}

#pragma mark - Methods

- (id)retain {
  // NXConstantString is immutable, so we return self
  return self;
}

- (void)release {
  // NXConstantString is immutable, so we do nothing
}

- (BOOL)isEqual:(id)anObject {
  if (self == anObject) {
    return YES;
  }
  if ([anObject class] == [self class]) {
    if (self->_length != ((NXConstantString *)anObject)->_length) {
      return NO;
    }
    return (memcmp(self->_data, ((NXConstantString *)anObject)->_data,
                   self->_length) == 0);
  }
  return NO;
}

@end
