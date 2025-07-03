#include <objc/objc.h>
#include <tests/tests.h>

// enumeration-1 Test using a bitfield enumeration ivar.  */

typedef enum { black, white } color;

@interface TestClass: Object {
  color c:2;
}

-(color) color;
-(void) setColor: (color)a;

@end

@implementation TestClass

-(color) color {
  return c;
}

-(void) setColor:(color)a {
  c = a;
}

@end

int main(void) {
  TestClass* c = [[TestClass alloc] init];

  [c setColor: black];
  test_assert([c color] == black);

  [c setColor: white];
  test_assert([c color] == white);

  [c setColor: black];
  test_assert([c color] == black);

  [c dealloc];

  return 0;
}

