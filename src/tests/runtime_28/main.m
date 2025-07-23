#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_28(void);

int main(void) { return TestMain("test_runtime_28", test_runtime_28); }

/* Test defining a protocol, a class adopting it in a category */

@protocol Evaluating
- (int)importance;
@end

@interface Feature : Object
@end

@implementation Feature
@end

@interface Feature (EvaluatingProtocol) <Evaluating>
@end

@implementation Feature (EvaluatingProtocol)
- (int)importance {
  return 1000;
}
@end

int test_runtime_28(void) {
  id<Evaluating> object = [[Feature alloc] init];
  test_assert(object != nil);
  test_assert([(Object *)object conformsTo:@protocol(Evaluating)]);
  test_assert([object importance] == 1000);
  [(Object *)object dealloc];
  return 0;
}
