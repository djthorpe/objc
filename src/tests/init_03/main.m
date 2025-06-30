#include <string.h>
#include <objc/objc.h>

int main() {
    NXConstantString* str1 = @"tests";
    assert([str1 length] == 5);
    assert(strcmp([str1 cStr], "tests") == 0);

    NXConstantString* str2 = @"tests";
    assert([str2 length] == 5);
    assert(strcmp([str2 cStr], "tests") == 0);

    NXConstantString* str3 = @"tests3";
    assert([str3 length] == 6);
    assert(strcmp([str3 cStr], "tests3") == 0);

    // isEqual
    assert([str1 isEqual:str2]);
    assert(![str1 isEqual:str3]);
    assert(![str2 isEqual:str3]);

    // Return success
    return 0;
}
