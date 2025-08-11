#include <objc/objc.h>
#include <tests/tests.h>

// enumeration-2 Test using a bitfield enumeration ivar.  */

typedef enum { black, white } color;

typedef struct {
  color a:2;
  color b:2;
} color_couple;

@interface TestClass: Object {
  color_couple *c;
}

-(color_couple* )colorCouple;
-(void)setColorCouple: (color_couple* )a;

@end

@implementation TestClass

-(color_couple *) colorCouple {
  return c;
}

-(void) setColorCouple:(color_couple *)a {
  c = a;
}

@end


int main(void) {
  color_couple cc;
  TestClass* c = [[TestClass alloc] init];

  cc.a = black;
  cc.b = white;

  [c setColorCouple: &cc];
  test_assert([c colorCouple] == &cc);

  [c dealloc];

  return 0;
}
