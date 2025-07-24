#include <objc/objc.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

int test_runtime_32(void);

int main(void) { return TestMain("test_runtime_32", test_runtime_32); }

@protocol Foo
- (void)foo;
@end

int test_runtime_32(void) {
  Protocol *protocol = @protocol(Foo);
  test_assert(protocol != nil);
  test_assert(proto_getName((objc_protocol_t *)protocol) != NULL);
  test_cstrings_equal(proto_getName((objc_protocol_t *)protocol), "Foo");
  test_cstrings_equal(proto_getName((objc_protocol_t *)@protocol(Foo)), "Foo");
  test_cstrings_equal([Protocol name], "Protocol");
  test_assert([protocol isEqual:protocol]);
  return 0;
}
