#include <tests/tests.h>
#include <NXFoundation/NXFoundation.h>

int main() {
    // Create an instance of NXObject
    NXObject* object = [[NXObject alloc] init];
    test_assert(object != nil);
    test_assert([object class] == [NXObject class]);

    // Return success
    return 0;
}
