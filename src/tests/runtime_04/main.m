#include <objc/objc.h>
#include <tests/tests.h>
#include "Test.h"

int main() {
    // Allocate a test object with a value
    Test* test = [[Test alloc] initWithValue:42];
    test_assert(test != NULL);

    // Check the value
    test_assert([test value] == 42);

    // Set the value to 100
    [test setValue:100];
    test_assert([test value] == 100);

    // Return success
    return 0;
}
