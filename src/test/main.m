#include <stdio.h>
#include <objc/runtime.h>
#include <objc/Object.h>

@protocol Runnable
- (void) run;
@end

@interface Test : Object<Runnable> {
    int propertyValue;
}

@property (nonatomic, assign) int propertyValue;

// A class method
+ (void) classMethod;

// Run the test
- (void) run;

@end

@implementation Test
@synthesize propertyValue;

- (id) init {
    printf("[Test init]\n");
    return self;
}

+ (void) classMethod {
    printf("[Test classMethod]\n");
}

- (void) run {
    [self setPropertyValue:42];

    printf("[test run]\n");

    // insert code here...
    printf("Hello, World!\n");
}
@end

int main(int argc, const char * argv[]) {
//    @autoreleasepool {
        Test *test = [Test alloc];
//        [test run];
//    }
    return 0;
}
