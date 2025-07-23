#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_31(void);

int main(void) { return TestMain("test_runtime_31", test_runtime_31); }

/* Test defining two protocols, one incorporating the other one. */

@protocol Configuring
- (void)configure;
@end

@protocol Processing <Configuring>
- (void)process;
@end

/* A class adopting the protocol */
@interface Test : Object <Processing> {
@public
  BOOL didConfigure;
  BOOL didProcess;
}
@end

@implementation Test
- (void)configure {
  didConfigure = YES;
}
- (void)process {
  didProcess = YES;
}
@end

int test_runtime_31(void) {
  id<Processing> object = [[Test alloc] init];
  test_assert(object != nil);
  test_assert([(Object *)object conformsTo:@protocol(Processing)]);
  test_assert([(Object *)object conformsTo:@protocol(Configuring)]);

  [object configure];
  [object process];
  test_assert(((Test *)object)->didConfigure);
  test_assert(((Test *)object)->didProcess);

  [(Object *)object dealloc];

  return 0;
}
