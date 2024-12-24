#include <stdio.h>
#include <objc/runtime.h>
#include <objc/Object.h>

@interface Test : Object

@property (nonatomic, assign) int propertyValue;

// Run the test
- (void) run;

@end

@implementation Test

+ (void) classMethod {
    printf("[Test classMethod]\n");
}

- (void) run {
    [self setPropertyValue:42];

    // insert code here...
    printf("Hello, World!\n");
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        printf("[[Test alloc] init]\n");
        Test *test = [[Test alloc] init];
        printf("[test run]\n");
        [test run];
    }
    return 0;
}
