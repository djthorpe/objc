#include <objc/objc.h>
#include "Test1.h"

int main() {
    // Allocate a test object with a value
    Test1* test = [[Test1 alloc] initWithValue:42];
    assert(test != NULL);

    // Check the value
    assert([test value] == 42);
    assert([test value1] == 42);
    assert([test value2] == 0); // value2 should not be set

    // Set the value to 100
    [test setValue:100];
    assert([test value] == 100);

    [test setValue1:101];
    assert([test value] == 101);

    [test setValue2:102];
    assert([test value2] == 102);

    [test dealloc]; // Dispose of the object

    // Return success
    return 0;
}
