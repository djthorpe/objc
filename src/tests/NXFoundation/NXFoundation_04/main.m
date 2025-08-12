#include <NXFoundation/NXFoundation.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  // Create a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);
  test_assert([NXZone defaultZone] == zone);

  // NXString
  NXLog(@"\n\nTesting [NXString initWithString] with NXConstantString");
  NXString *str = [[NXString alloc] initWithString:@"Hello, World!"];
  test_assert(str != nil);
  test_assert([str isKindOfClass:[NXString class]]);
  test_assert(strcmp([str cStr], "Hello, World!") == 0);
  test_assert([str length] == 13);

  // NXString referencing another string
  NXLog(@"\n\nTesting [NXString initWithString] with NXString");
  NXString *otherStr = [[NXString alloc] initWithString:str];
  test_assert(otherStr != nil);
  test_assert([otherStr isKindOfClass:[NXString class]]);
  test_assert(strcmp([otherStr cStr], "Hello, World!") == 0);
  test_assert([otherStr length] == 13);

  // Deallocate the strings
  [str release];
  [otherStr release];

  // Free the zone
  [zone dealloc];
  test_assert([NXZone defaultZone] == nil);

  // Return success
  return 0;
}
