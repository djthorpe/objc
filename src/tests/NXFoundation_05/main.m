#include <string.h>
#include <tests/tests.h>
#include <NXFoundation/NXFoundation.h>

int main() {
    // Create a zone
    NXZone* zone = [NXZone zoneWithSize:0];
    test_assert(zone != nil);
    test_assert([NXZone defaultZone] == zone);

    NXObject* obj = [[NXObject alloc] init];
    test_stringsequal([obj description], @"NXObject");

    test_stringsequal([zone description], @"NXZone");

    // Free the zone
    [zone dealloc];
    test_assert([NXZone defaultZone] == nil);

    // Return success
    return 0;
}
