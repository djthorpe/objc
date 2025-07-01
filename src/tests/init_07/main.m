#include <objc/objc.h>
#include <tests/tests.h>
#include "Object+Description.h"

int main() {
    // Get the description from the category
    NXConstantString* description = [Object description];
    test_assert([description isEqual:@"Object"]);

    // TODO: The string should be deallocated
    [description dealloc];

    // Return success
    return 0;
}
