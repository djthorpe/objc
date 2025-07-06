#include <objc/objc.h>
#include <tests/tests.h>

@interface Foo : Object
+ (id) foo;
+ (id)bar;
@end

int foocalled = 0;
int barcalled = 0;

@implementation Foo
+ (id)foo
{
    test_assert(!foocalled);
    foocalled = 1;
    return self;
}

+ (id)bar
{
    test_assert(!barcalled);
    barcalled = 1;
    return self;
}
@end

int main()
{
    [[Foo foo] bar];
    return 0;
}
