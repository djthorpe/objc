#include <string.h>
#include <objc/objc.h>

int main() {
    [NXConstantString test];   

    NXConstantString* str = @"tests";
    assert([str length] == 5);
    assert(strcmp([str cStr], "tests") == 0);

    // Log the string    
    NSLog(@"object_03 %s...", [str cStr]);

    // Return success
    return 0;
}
