#include <objc/objc.h>
#include <tests/tests.h>
#include "Test.h"
#include "Test+Description.h"

int main() {
    // Allocate a test object with a value
    Test* test = [[Test alloc] initWithString:@"42"];
    test_assert(test != NULL);

    // Get the description from the category
    NXConstantString* description = [test description];
    test_assert([description isEqual:@"42"]);

    // Return success
    return 0;
}
