#include <objc/objc.h>
#include <string.h>

@implementation NXConstantString

#pragma mark - Lifecycle

- (id)initWithCString:(const char *)cStr {
  self = [super init];
  if (self != nil) {
    _data = cStr;
    _length = (unsigned int)strlen(cStr);
  }
  return self;
}

#pragma mark - Properties

- (const char *)cStr {
  return self->_data;
}

- (unsigned int)length {
  return self->_length;
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
