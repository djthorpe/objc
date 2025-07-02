#include <string.h>
#include <objc/objc.h>
#include <tests/tests.h>

int main() {
    NXConstantString* str1 = @"tests";
    test_assert([str1 length] == 5);
    test_assert(strcmp([str1 cStr], "tests") == 0);

    NXConstantString* str2 = @"tests";
    test_assert([str2 length] == 5);
    test_assert(strcmp([str2 cStr], "tests") == 0);

    NXConstantString* str3 = @"tests3";
    test_assert([str3 length] == 6);
    test_assert(strcmp([str3 cStr], "tests3") == 0);

    // isEqual
    test_assert([str1 isEqual:str2]);
    test_assert(![str1 isEqual:str3]);
    test_assert(![str2 isEqual:str3]);

    // Return success
    return 0;
}
