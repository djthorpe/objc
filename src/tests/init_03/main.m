#include <string.h>
#include <objc/objc.h>

int main() {
    [NXConstantString test];
    
    
    NXConstantString *str = @"tests";
    NSLog(@"object_03 %s...", [str cStr]);
    assert([str length] == 5);
    assert(strcmp([str cStr], "tests") == 0);

    // Return success
    return 0;
}
