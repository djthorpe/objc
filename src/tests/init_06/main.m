#include <objc/objc.h>
#include "Test.h"

int main() {
    // Allocate a test object with a value
    Test* test = [[Test alloc] initWithString:@"42"];
    assert(test != NULL);

    // Get the description from the category
    NXConstantString* description = [test description];
    assert([description isEqual:@"42"]);

    // Return success
    return 0;
}
