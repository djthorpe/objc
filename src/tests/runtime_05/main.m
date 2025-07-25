#include <objc/objc.h>
#include <tests/tests.h>
#include "Test1.h"

int main() {
    // Allocate a test object with a value
    Test1* test = [[Test1 alloc] initWithValue:42];
    test_assert(test != NULL);

    // Check the value
    test_assert([test value] == 42);
    test_assert([test value1] == 42);
    test_assert([test value2] == 0); // value2 should not be set

    // Set the value to 100
    [test setValue:100];
    test_assert([test value] == 100);

    [test setValue1:101];
    test_assert([test value] == 101);

    [test setValue2:102];
    test_assert([test value2] == 102);

    [test dealloc]; // Dispose of the object

    // Return success
    return 0;
}
