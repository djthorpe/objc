#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

@interface TestA : NXObject
@end

@implementation TestA
@end

@interface TestB : TestA
@end

@implementation TestB
@end

int main() {
  // Create a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);
  test_assert([NXZone defaultZone] == zone);

  // NXObject
  NXObject *object = [[NXObject alloc] init];
  test_assert(object != nil);
  test_assert([object class] != [TestA class]);
  test_assert([object class] != [TestB class]);
  test_assert([object class] == [NXObject class]);
  test_assert([object isKindOfClass:[TestA class]] == NO);
  test_assert([object isKindOfClass:[TestB class]] == NO);
  test_assert([object isKindOfClass:[NXObject class]]);
  test_assert([object isKindOfClass:[Object class]]);
  [object release];

  // TestA
  TestA *testa = [[TestA alloc] init];
  test_assert(testa != nil);
  test_assert([testa class] == [TestA class]);
  test_assert([testa class] != [TestB class]);
  test_assert([testa isKindOfClass:[TestA class]]);
  test_assert([object isKindOfClass:[TestB class]] == NO);
  test_assert([testa isKindOfClass:[NXObject class]]);
  test_assert([testa isKindOfClass:[Object class]]);
  [testa release];

  // TestB
  TestB *testb = [[TestB alloc] init];
  test_assert(testb != nil);
  test_assert([testb class] != [TestA class]);
  test_assert([testb class] == [TestB class]);
  test_assert([testb isKindOfClass:[TestB class]]);
  test_assert([testb isKindOfClass:[TestA class]]);
  test_assert([testb isKindOfClass:[NXObject class]]);
  test_assert([testb isKindOfClass:[Object class]]);
  [testb release];

  // Free the zone
  // TODO: Use an autorelease pool to ensure proper memory management
  [zone dealloc];
  test_assert([NXZone defaultZone] == nil);

  // Return success
  return 0;
}
