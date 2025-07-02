#include <tests/tests.h>
#include <NXFoundation/NXFoundation.h>

int main() {
    // Create a zone
    NXZone* zone = [NXZone zoneWithSize:0];
    test_assert(zone != nil);
    test_assert([NXZone defaultZone] == zone);

    // Create an instance of NXObject
    NXObject* object = [[NXObject alloc] init];
    test_assert(object != nil);
    test_assert([object class] == [NXObject class]);

    // Free the zone
    [zone dealloc];
    test_assert([NXZone defaultZone] == nil);

    // Return success
    return 0;
}
