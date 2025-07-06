#include <string.h>
#include <tests/tests.h>
#include <NXFoundation/NXFoundation.h>

int main() {
    // Create a zone, and autorelease pool
    NXZone* zone = [NXZone zoneWithSize:0];
    NXAutoreleasePool* pool = [[NXAutoreleasePool alloc] init];
    test_assert(pool != nil);
    test_assert([NXAutoreleasePool currentPool] == pool);

    // Add 100 objects to the pool
    for (int i = 0; i < 100; i++) {
        NXObject* obj = [[[NXObject alloc] init] autorelease];
        test_assert(obj != nil);
    }

    // Free the pool and the zone
    [pool release];
    test_assert([NXAutoreleasePool currentPool] == nil);
    [zone dealloc];
    test_assert([NXZone defaultZone] == nil);

    // Return success
    return 0;
}
