#include <NXFoundation/NXFoundation.h>

int main() {
    // Create an instance of NXObject
    NXObject* object = [[NXObject alloc] init];
    assert(object != nil);
    assert([object class] == [NXObject class]);

    // Return success
    return 0;
}
