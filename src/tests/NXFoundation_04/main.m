#include <string.h>
#include <tests/tests.h>
#include <NXFoundation/NXFoundation.h>

int main() {
    // Create a zone
    NXZone* zone = [NXZone zoneWithSize:0];
    test_assert(zone != nil);
    test_assert([NXZone defaultZone] == zone);

    // NXString referencing NXObject
    NXLog(@"\n\nTesting [NXString initWithString] with NXObject");
    NXString* null = [[NXString alloc] initWithString:[[NXObject alloc] init]];
    test_assert(null == nil);

    // NXString referencing NXObject
    NXLog(@"\n\nTesting [NXString initWithString] with Object");
    NXString* null2 = [[NXString alloc] initWithString:[[Object alloc] init]];
    test_assert(null2 == nil);

    // NXString
    NXLog(@"\n\nTesting [NXString initWithString] with NXConstantString");
    NXString* str = [[NXString alloc] initWithString:@"Hello, World!"];
    test_assert(str != nil);
    test_assert([str isKindOfClass:[NXString class]]);
    test_assert(strcmp([str cStr], "Hello, World!") == 0);
    test_assert([str length] == 13);

    // NXString referencing another string
    NXLog(@"\n\nTesting [NXString initWithString] with NXString");
    NXString* otherStr = [[NXString alloc] initWithString:str];
    test_assert(otherStr != nil);
    test_assert([otherStr isKindOfClass:[NXString class]]);
    test_assert(strcmp([otherStr cStr], "Hello, World!") == 0);
    test_assert([otherStr length] == 13);

    // Deallocate the strings
    NXLog(@"\n\nReleasing 1 strings %p", null);
    [null release];
    [null2 release];
    [str release];
    [otherStr release];

    // Free the zone
    [zone dealloc];
    test_assert([NXZone defaultZone] == nil);

    // Return success
    return 0;
}
