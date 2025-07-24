#include <NXFoundation/NXFoundation.h>
#include <stdio.h>

int main() {
  NXAutoreleasePool *pool = [[NXAutoreleasePool alloc] init];

  printf("Testing zero optimization...\n");

  // Test signed int64 zero optimization
  NXNumber *zero1 = [NXNumber numberWithInt64:0];
  NXNumber *zero2 = [NXNumber numberWithInt64:0];

  printf("Zero Int64 #1: %p\n", (void *)zero1);
  printf("Zero Int64 #2: %p\n", (void *)zero2);
  printf("Same instance: %s\n", (zero1 == zero2) ? "YES" : "NO");

  // Test unsigned int64 zero optimization
  NXNumber *uzero1 = [NXNumber numberWithUnsignedInt64:0U];
  NXNumber *uzero2 = [NXNumber numberWithUnsignedInt64:0U];

  printf("Zero UInt64 #1: %p\n", (void *)uzero1);
  printf("Zero UInt64 #2: %p\n", (void *)uzero2);
  printf("Same instance: %s\n", (uzero1 == uzero2) ? "YES" : "NO");

  // Test cross-type zero equality
  printf("Int64(0) == UInt64(0): %s\n", [zero1 isEqual:uzero1] ? "YES" : "NO");

  // Test non-zero values (should be different instances)
  NXNumber *one1 = [NXNumber numberWithInt64:1];
  NXNumber *one2 = [NXNumber numberWithInt64:1];

  printf("One #1: %p\n", (void *)one1);
  printf("One #2: %p\n", (void *)one2);
  printf("Same instance: %s\n", (one1 == one2) ? "YES" : "NO");

  [pool drain];
  return 0;
}
