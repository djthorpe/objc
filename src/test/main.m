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

- (id) init {
    printf("[Test init]\n");
    return self;
}

- (void) run {
    [self setPropertyValue:42];

    printf("[test run]\n");

    // insert code here...
    printf("Hello, World!\n");
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        Test *test = [[Test alloc] init];
        [test run];
    }
    return 0;
}
