#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_27(void);

int main(void) { return TestMain("test_runtime_27", test_runtime_27); }

/* Test defining two protocols, a class adopting both of them,
   and using an object of type `id <Protocol1, Protocol2>' */

@protocol Enabling
- (BOOL)isEnabled;
- (void)setEnabled:(BOOL)flag;
@end

@protocol Evaluating
- (int)importance;
@end

@interface Feature : Object <Enabling, Evaluating> {
  const char *name;
  BOOL isEnabled;
}
@end

@implementation Feature
- (BOOL)isEnabled {
  return isEnabled;
}
- (void)setEnabled:(BOOL)flag {
  isEnabled = flag;
}
- (int)importance {
  return 1000;
}
@end

int test_runtime_27(void) {
  id<Enabling, Evaluating> object = [[Feature alloc] init];

  test_assert([(Object *)object conformsTo:@protocol(Enabling)]);
  test_assert([(Object *)object conformsTo:@protocol(Evaluating)]);
  test_assert([[(Object *)object class] conformsTo:@protocol(Enabling)]);
  test_assert([[(Object *)object class] conformsTo:@protocol(Evaluating)]);
  test_assert([@protocol(Evaluating) isEqual:@protocol(Enabling)] == NO);
  test_assert([@protocol(Enabling) conformsTo:@protocol(Evaluating)] == NO);
  test_assert([@protocol(Evaluating) conformsTo:@protocol(Enabling)] == NO);
  test_assert([@protocol(Enabling) conformsTo:@protocol(Enabling)] == YES);
  test_assert([@protocol(Evaluating) conformsTo:@protocol(Evaluating)] == YES);

  [object setEnabled:YES];
  test_assert(object != nil);
  test_assert([object isEnabled]);
  test_assert([object importance] == 1000);
  [(Object *)object dealloc];

  return 0;
}
