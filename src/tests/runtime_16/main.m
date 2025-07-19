#include <objc/objc.h>
#include <stdio.h>
#include <string.h>
#include <tests/tests.h>

// encode-1: Test very simple @encode
// https://github.com/gnustep/libobjc/blob/master/objc/objc-api.h
/*
#define _C_ID       '@'
#define _C_CLASS    '#'
#define _C_SEL      ':'
#define _C_CHR      'c'
#define _C_UCHR     'C'
#define _C_SHT      's'
#define _C_USHT     'S'
#define _C_INT      'i'
#define _C_UINT     'I'
#define _C_LNG      'l'
#define _C_ULNG     'L'
#define _C_LNG_LNG  'q'
#define _C_ULNG_LNG 'Q'
#define _C_FLT      'f'
#define _C_DBL      'd'
#define _C_BFLD     'b'
#define _C_BOOL	    'B'
#define _C_VOID     'v'
#define _C_UNDEF    '?'
#define _C_PTR      '^'
#define _C_CHARPTR  '*'
#define _C_ATOM     '%'
#define _C_ARY_B    '['
#define _C_ARY_E    ']'
#define _C_UNION_B  '('
#define _C_UNION_E  ')'
#define _C_STRUCT_B '{'
#define _C_STRUCT_E '}'
#define _C_VECTOR   '!'
#define _C_COMPLEX   'j'
*/

// NOTE: Currently, ObjC encodes 'long' as 'l' (and 'unsigned long' as 'L') if
// sizeof(long) == sizeof(int) On 64-bit targets, where sizeof(long) == 2 *
// sizeof(int), 'long' and 'unsigned long' get encoded as 'q' and 'Q' instead.

int main(void) {
  test_assert(strcmp("c", @encode(char)) == 0);
  test_assert(strcmp("C", @encode(unsigned char)) == 0);
  test_assert(strcmp("s", @encode(short)) == 0);
  test_assert(strcmp("S", @encode(unsigned short)) == 0);
  test_assert(strcmp("i", @encode(int)) == 0);
  test_assert(strcmp("I", @encode(unsigned int)) == 0);
  test_assert(strcmp("q", @encode(long long)) == 0);
  test_assert(strcmp("Q", @encode(unsigned long long)) == 0);
  test_assert(strcmp("f", @encode(float)) == 0);
  test_assert(strcmp("d", @encode(double)) == 0);
  test_assert(strcmp("B", @encode(BOOL)) == 0);
  test_assert(strcmp("*", @encode(char *)) == 0);
  test_assert(strcmp("@", @encode(id)) == 0);
  test_assert(strcmp("@", @encode(Object *)) == 0);
  test_assert(strcmp(":", @encode(SEL)) == 0);
  test_assert(strcmp("^v", @encode(void *)) == 0);

  // Debug what we're actually getting for long types
  printf("@encode(long) = '%s'\n", @encode(long));
  printf("@encode(unsigned long) = '%s'\n", @encode(unsigned long));

  // On 64-bit systems, long might be 'q' (long long) instead of 'l'
  // Let's check both possibilities
  const char *long_encoding = @encode(long);
  const char *ulong_encoding = @encode(unsigned long);

  // Accept either 'l' (32-bit) or 'q' (64-bit) for long
  test_assert(strcmp("l", long_encoding) == 0 ||
              strcmp("q", long_encoding) == 0);
  // Accept either 'L' (32-bit) or 'Q' (64-bit) for unsigned long
  test_assert(strcmp("L", ulong_encoding) == 0 ||
              strcmp("Q", ulong_encoding) == 0);

  return 0;
}
