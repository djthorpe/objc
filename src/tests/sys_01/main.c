#include <runtime-sys/sys.h>
#include <tests/tests.h>

// Helper function to test sys_vsprintf
size_t helper_test_sys_vsprintf(char *buf, size_t sz, const char *format, ...) {
  va_list args;
  va_start(args, format);
  size_t result = sys_vsprintf(buf, sz, format, args);
  va_end(args);
  return result;
}

// Forward declaration
int test_sys_01(void);

int main(void) {
  // Run sys_printf tests
  return TestMain("test_sys_01", test_sys_01);
}

int test_sys_01(void) {
  // Test 1: Output a message to the console
  sys_puts("Test 1: sys_printf basic output\n");
  do {
    size_t len = sys_printf("Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // Test 2: Output a formatted message
  sys_puts("Test 2: sys_sprintf basic formatting\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // Test 3: sprintf with no buffer
  sys_puts("Test 3: sys_sprintf with NULL buffer\n");
  do {
    size_t len = sys_sprintf(NULL, 0, "Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // Test 4: % at the end
  sys_puts("Test 4: sys_printf with % at end\n");
  do {
    size_t len = sys_printf("Hello, World!%");
    test_assert(len == 14);
  } while (0);

  // Test 5: %% at the end
  sys_puts("Test 5: sys_printf with %% at end\n");
  do {
    size_t len = sys_printf("Hello, World!%%");
    test_assert(len == 14);
  } while (0);

  // Test 6: %% in the middle
  sys_puts("Test 6: sys_printf with %% in middle\n");
  do {
    size_t len = sys_printf("Hello%% World!\n");
    test_assert(len == 14);
  } while (0);

  // Test 7: %s with zero-length string
  sys_puts("Test 7: sys_printf with empty string\n");
  do {
    size_t len = sys_printf("Hello%s, World!\n", "");
    test_assert(len == 14);
  } while (0);

  // Test 8: %s with c-string
  sys_puts("Test 8: sys_printf with string substitution\n");
  do {
    size_t len = sys_printf("Hello, %s\n", "World!");
    test_assert(len == 14);
  } while (0);

  // Test 9: %s with NULL string
  sys_puts("Test 9: sys_printf with NULL string\n");
  do {
    size_t len = sys_printf("Hello%s World!\n", NULL);
    test_assert(len == 18);
  } while (0);

  // Test 10: decimal format
  sys_puts("Test 10: sys_printf with decimal format\n");
  do {
    size_t len = sys_printf("42==%d\n", 42);
    test_assert(len == 7);
  } while (0);

  // Test 11: negative decimal
  sys_puts("Test 11: sys_printf with negative decimal\n");
  do {
    size_t len = sys_printf("-42==%d\n", -42);
    test_assert(len == 9);
  } while (0);

  // Test 12: positive decimal with + flag
  sys_puts("Test 12: sys_printf with positive sign flag\n");
  do {
    size_t len = sys_printf("+42==%+d\n", 42);
    test_assert(len == 9); // "+42==+42\n" = 9 characters
  } while (0);

  // Test 13: negative decimal with + flag
  sys_puts("Test 13: sys_printf negative with + flag\n");
  do {
    size_t len = sys_printf("-42==%+d\n", -42);
    test_assert(len == 9); // "-42==-42\n" = 9 characters
  } while (0);

  // Test 14: decimal with sprintf
  sys_puts("Test 14: sys_sprintf with decimal format\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "42==%d", 42);
    test_cstrings_equal("42==42", buffer);
    test_assert(len == 6);
  } while (0);

  // Test 15: hexadecimal format (lowercase)
  sys_puts("Test 15: sys_printf with hex format (lowercase)\n");
  do {
    size_t len = sys_printf("255==%x\n", 255);
    test_assert(len == 8); // "255==ff\n" = 8 characters
  } while (0);

  // Test 16: hexadecimal format (uppercase)
  sys_puts("Test 16: sys_printf with hex format (uppercase)\n");
  do {
    size_t len = sys_printf("255==%X\n", 255);
    test_assert(len == 8); // "255==FF\n" = 8 characters
  } while (0);

  // Test 17: hexadecimal with prefix (lowercase)
  sys_puts("Test 17: sys_printf with hex prefix (lowercase)\n");
  do {
    size_t len = sys_printf("255==%#x\n", 255);
    test_assert(len == 10); // "255==0xff\n" = 10 characters
  } while (0);

  // Test 18: hexadecimal with prefix (uppercase)
  sys_puts("Test 18: sys_printf with hex prefix (uppercase)\n");
  do {
    size_t len = sys_printf("255==%#X\n", 255);
    test_assert(len == 10); // "255==0XFF\n" = 10 characters
  } while (0);

  // Test 19: binary format
  sys_puts("Test 19: sys_printf with binary format\n");
  do {
    size_t len = sys_printf("15==%b\n", 15);
    test_assert(len == 9); // "15==1111\n" = 9 characters
  } while (0);

  // Test 20: binary with prefix
  sys_puts("Test 20: sys_printf with binary prefix\n");
  do {
    size_t len = sys_printf("15==%#b\n", 15);
    test_assert(len == 11); // "15==0b1111\n" = 11 characters
  } while (0);

  // Test 21: octal format
  sys_puts("Test 21: sys_printf with octal format\n");
  do {
    size_t len = sys_printf("64==%o\n", 64);
    test_assert(len == 8); // "64==100\n" = 8 characters
  } while (0);

  // Test 22: octal with prefix
  sys_puts("Test 22: sys_printf with octal prefix\n");
  do {
    size_t len = sys_printf("64==%#o\n", 64);
    test_assert(len == 9); // "64==0100\n" = 9 characters
  } while (0);

  // Test 23: unsigned decimal
  sys_puts("Test 23: sys_printf with unsigned decimal\n");
  do {
    size_t len = sys_printf("4294967295==%u\n", 4294967295U);
    test_assert(len == 23); // "4294967295==4294967295\n" = 23 characters
  } while (0);

  // Test 24: character format
  sys_puts("Test 24: sys_printf with character format\n");
  do {
    size_t len = sys_printf("A==%c\n", 'A');
    test_assert(len == 5); // "A==A\n" = 5 characters
  } while (0);

  // Test 25: multiple character format
  sys_puts("Test 25: sys_printf with multiple characters\n");
  do {
    size_t len = sys_printf("char: %c %c %c\n", 'X', 'Y', 'Z');
    test_assert(len == 12); // "char: X Y Z\n" = 12 characters
  } while (0);

  // Test 26: special characters
  sys_puts("Test 26: sys_printf with special characters\n");
  do {
    size_t len = sys_printf("special: %c%c%c\n", ' ', '\t', '!');
    test_assert(
        len ==
        13); // "special:  	!\n" = 13 characters (space, tab, exclamation)
  } while (0);

  // Test 27: numeric character codes
  sys_puts("Test 27: sys_sprintf with numeric character codes\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%c%c%c", '0', '5', '9');
    test_cstrings_equal("059", buffer);
    test_assert(len == 3);
  } while (0);

  // Test 28: character with sprintf
  sys_puts("Test 28: sys_sprintf with character\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Letter: %c", 'Q');
    test_cstrings_equal("Letter: Q", buffer);
    test_assert(len == 9);
  } while (0);

  // Test 29: ASCII control characters
  sys_puts("Test 29: sys_printf with ASCII control characters\n");
  do {
    size_t len =
        sys_printf("ASCII: %c %c %c\n", 32, 65, 126); // space, 'A', '~'
    test_assert(len == 13); // "ASCII:   A ~\n" = 13 characters
  } while (0);

  // Test 30: character promotion from int
  sys_puts("Test 30: sys_sprintf with character promotion\n");
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%c%c", (int)'H', (int)'i');
    test_cstrings_equal("Hi", buffer);
    test_assert(len == 2);
  } while (0);

  // Test 31: punctuation and symbols
  sys_puts("Test 31: sys_printf with punctuation and symbols\n");
  do {
    size_t len = sys_printf("symbols: %c%c%c%c%c\n", '@', '#', '$', '%', '&');
    test_assert(len == 15); // "symbols: @#$%&\n" = 15 characters
  } while (0);

  // Test 32: mixed characters and format specifiers
  sys_puts("Test 32: sys_sprintf with mixed formats\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Mix: %d%c%s%c%d", 1, ':',
                             "test", ':', 2);
    test_cstrings_equal("Mix: 1:test:2", buffer);
    test_assert(len == 13);
  } while (0);

  // Test 33: character with zero value (null character)
  sys_puts("Test 33: sys_sprintf with null character\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "null%cend", 0);
    test_assert(len ==
                8); // "null\0end" includes all 8 characters, even though the
                    // null character at position 4 terminates the C string.
    // Note: sys_sprintf counts all characters written to the buffer, including
    // those after the null character, in its return value. The buffer will
    // appear as "null" when treated as a C string, but the length returned by
    // sys_sprintf reflects all characters.
  } while (0);

  // Test 34: long integers
  sys_puts("Test 34: sys_printf with long integers\n");
  do {
    size_t len = sys_printf("123456789==%ld\n", 123456789L);
    test_assert(len == 21); // "123456789==123456789\n" = 21 characters
  } while (0);

  // Test 35: long unsigned integers
  sys_puts("Test 35: sys_printf with long unsigned integers\n");
  do {
    size_t len = sys_printf("123456789==%lu\n", 123456789UL);
    test_assert(len == 21); // "123456789==123456789\n" = 21 characters
  } while (0);

  // Test 36: size_t integers
  sys_puts("Test 36: sys_printf with size_t integers\n");
  do {
    size_t len = sys_printf("12345==%zd\n", (size_t)12345);
    test_assert(len == 13); // "12345==12345\n" = 13 characters
  } while (0);

  // Test 37: size_t unsigned integers
  sys_puts("Test 37: sys_printf with size_t unsigned integers\n");
  do {
    size_t len = sys_printf("12345==%zu\n", (size_t)12345);
    test_assert(len == 13); // "12345==12345\n" = 13 characters
  } while (0);

  // Test 38: sprintf with hexadecimal
  sys_puts("Test 38: sys_sprintf with hexadecimal\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "0x%x", 255);
    test_cstrings_equal("0xff", buffer);
    test_assert(len == 4);
  } while (0);

  // Test 39: sprintf with binary
  sys_puts("Test 39: sys_sprintf with binary\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%#b", 7);
    test_cstrings_equal("0b111", buffer);
    test_assert(len == 5);
  } while (0);

  // Test 40: sprintf with octal
  sys_puts("Test 40: sys_sprintf with octal\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%#o", 8);
    test_cstrings_equal("010", buffer);
    test_assert(len == 3);
  } while (0);

  // Test 41: edge case - zero with different formats
  sys_puts("Test 41: sys_printf with zero in different formats\n");
  do {
    size_t len = sys_printf("0: dec=%d hex=%x bin=%b oct=%o\n", 0, 0, 0, 0);
    test_assert(len == 27); // "0: dec=0 hex=0 bin=0 oct=0\n" = 27 characters
  } while (0);

  // Test 42: edge case - maximum values
  sys_puts("Test 42: sys_printf with maximum int value\n");
  do {
    size_t len = sys_printf("max int: %d\n", 2147483647);
    test_assert(len == 20); // "max int: 2147483647\n" = 20 characters
  } while (0);

  // Test 43: mixed format specifiers in one call
  sys_puts("Test 43: sys_sprintf with mixed format specifiers\n");
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer),
                    "Dec:%d Hex:%x Bin:%b Oct:%o Char:%c", 42, 42, 42, 42, 'Z');
    test_cstrings_equal("Dec:42 Hex:2a Bin:101010 Oct:52 Char:Z", buffer);
    test_assert(len == 38);
  } while (0);

  // Test 44: negative values
  sys_puts("Test 44: sys_printf with negative values\n");
  do {
    size_t len = sys_printf("negative: %d\n", -123);
    test_assert(len == 15); // "negative: -123\n" = 15 characters
  } while (0);

  // Test 45: negative with positive sign flag
  sys_puts("Test 45: sys_printf negative with positive sign flag\n");
  do {
    size_t len = sys_printf("negative: %+d\n", -456);
    test_assert(len == 15); // "negative: -456\n" = 15 characters
  } while (0);

  // Test 46: negative long integers
  sys_puts("Test 46: sys_printf with negative long integers\n");
  do {
    // Use a negative long value that fits in both 32-bit and 64-bit systems
    size_t len = sys_printf("negative long: %ld\n", -123456789L);
    test_assert(len == 26); // "negative long: -123456789\n" = 26 characters
  } while (0);

  // Test 47: negative with sprintf
  sys_puts("Test 47: sys_sprintf with negative values\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "neg:%d pos:%d", -42, 42);
    test_cstrings_equal("neg:-42 pos:42", buffer);
    test_assert(len == 14);
  } while (0);

  // Test 48: very negative numbers (near INT_MIN)
  sys_puts("Test 48: sys_printf with very negative numbers\n");
  do {
    size_t len = sys_printf("min int: %d\n", -2147483647);
    test_assert(len == 21); // "min int: -2147483647\n" = 21 characters
  } while (0);

  // Test 49: INT_MIN edge case
  sys_puts("Test 49: sys_printf with INT_MIN edge case\n");
  do {
    size_t len = sys_printf("int min: %d\n", (-2147483647 - 1));
    test_assert(len == 21); // "int min: -2147483648\n" = 21 characters
  } while (0);

  // Test 50: negative long minimum (LONG_MIN edge case)
  sys_puts("Test 50: sys_sprintf with LONG_MIN edge case\n");
  do {
    char buffer[100];
    // Use platform-appropriate LONG_MIN value
    if (sizeof(long) == 4) {
      // 32-bit long: LONG_MIN = -2147483648
      size_t len =
          sys_sprintf(buffer, sizeof(buffer), "%ld", (-2147483647L - 1));
      test_cstrings_equal("-2147483648", buffer);
      test_assert(len == 11); // "-2147483648" = 11 characters
    } else {
      // 64-bit long: LONG_MIN = -9223372036854775808
      size_t len = sys_sprintf(buffer, sizeof(buffer), "%ld",
                               (-9223372036854775807L - 1));
      test_cstrings_equal("-9223372036854775808", buffer);
      test_assert(len == 20); // "-9223372036854775808" = 20 characters
    }
  } while (0);

  // Test 51: multiple negative values in one call
  sys_puts("Test 51: sys_printf with multiple negative values\n");
  do {
    size_t len = sys_printf("a:%d b:%d c:%d\n", -1, -22, -333);
    test_assert(len == 18); // "a:-1 b:-22 c:-333\n" = 18 characters
  } while (0);

  // Test 52: zero vs negative zero
  sys_puts("Test 52: sys_printf with zero vs negative zero\n");
  do {
    size_t len = sys_printf("zero: %d, neg-zero: %d\n", 0, -0);
    test_assert(len == 21); // "zero: 0, neg-zero: 0\n" = 21 characters
  } while (0);

  // Test 53: negative with positive sign flag (sprintf)
  sys_puts("Test 53: sys_sprintf negative with positive sign flag\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "neg:%+d zero:%+d pos:%+d",
                             -5, 0, 5);
    test_cstrings_equal("neg:-5 zero:+0 pos:+5", buffer);
    test_assert(len == 21);
  } while (0);

  // Test 54: unsigned wraparound with negative
  sys_puts("Test 54: sys_sprintf unsigned wraparound with negative\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "unsigned -1: %u", -1);
    test_cstrings_equal("unsigned -1: 4294967295",
                        buffer); // Two's complement wraparound
    test_assert(len == 23);
  } while (0);

  // Test 55: hex with negative values (unsigned behavior)
  sys_puts("Test 55: sys_sprintf hex with negative values\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "hex -1: %x", -1);
    test_cstrings_equal("hex -1: ffffffff",
                        buffer); // Two's complement representation
    test_assert(len == 16);
  } while (0);

  // ===== WIDTH SPECIFIER TESTS =====

  // Test 56: string width - right aligned (default)
  sys_puts("Test 56: string width right aligned\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%10s'", "hello");
    test_cstrings_equal("'     hello'", buffer);
    test_assert(len == 12); // quotes + 5 spaces + 5 chars = 12
  } while (0);

  // Test 57: string width - left aligned
  sys_puts("Test 57: string width left aligned\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%-10s'", "hello");
    test_cstrings_equal("'hello     '", buffer);
    test_assert(len == 12); // quotes + 5 chars + 5 spaces = 12
  } while (0);

  // Test 58: string width - exact fit
  sys_puts("Test 58: string width exact fit\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%5s'", "hello");
    test_cstrings_equal("'hello'", buffer);
    test_assert(len == 7); // quotes + 5 chars = 7
  } while (0);

  // Test 59: string width - smaller than string (no truncation)
  sys_puts("Test 59: string width smaller than string\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%3s'", "hello");
    test_cstrings_equal("'hello'", buffer);
    test_assert(len == 7); // quotes + 5 chars = 7
  } while (0);

  // Test 60: character width - right aligned
  sys_puts("Test 60: character width right aligned\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%5c'", 'A');
    test_cstrings_equal("'    A'", buffer);
    test_assert(len == 7); // quotes + 4 spaces + 1 char = 7
  } while (0);

  // Test 61: character width - left aligned
  sys_puts("Test 61: character width left aligned\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%-5c'", 'A');
    test_cstrings_equal("'A    '", buffer);
    test_assert(len == 7); // quotes + 1 char + 4 spaces = 7
  } while (0);

  // Test 62: integer width - right aligned with spaces
  sys_puts("Test 62: integer width right aligned with spaces\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%8d'", 42);
    test_cstrings_equal("'      42'", buffer);
    test_assert(len == 10); // quotes + 6 spaces + 2 digits = 10
  } while (0);

  // Test 63: integer width - left aligned
  sys_puts("Test 63: integer width left aligned\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%-8d'", 42);
    test_cstrings_equal("'42      '", buffer);
    test_assert(len == 10); // quotes + 2 digits + 6 spaces = 10
  } while (0);

  // Test 64: integer width - zero padded
  sys_puts("Test 64: integer width zero padded\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08d'", 42);
    test_cstrings_equal("'00000042'", buffer);
    test_assert(len == 10); // quotes + 8 digits = 10
  } while (0);

  // Test 65: negative integer with zero padding
  sys_puts("Test 65: negative integer with zero padding\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08d'", -42);
    test_cstrings_equal("'-0000042'", buffer);
    test_assert(len == 10); // quotes + sign + 7 digits = 10
  } while (0);

  // Test 66: positive sign with width and zero padding
  sys_puts("Test 66: positive sign with width and zero padding\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%+08d'", 42);
    test_cstrings_equal("'+0000042'", buffer);
    test_assert(len == 10); // quotes + sign + 7 digits = 10
  } while (0);

  // Test 67: hexadecimal width - space padded
  sys_puts("Test 67: hexadecimal width space padded\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%8x'", 255);
    test_cstrings_equal("'      ff'", buffer);
    test_assert(len == 10); // quotes + 6 spaces + 2 hex = 10
  } while (0);

  // Test 68: hexadecimal width - zero padded
  sys_puts("Test 68: hexadecimal width zero padded\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08x'", 255);
    test_cstrings_equal("'000000ff'", buffer);
    test_assert(len == 10); // quotes + 8 hex = 10
  } while (0);

  // Test 69: hexadecimal with prefix and width
  sys_puts("Test 69: hexadecimal with prefix and width\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#10x'", 255);
    test_cstrings_equal("'      0xff'", buffer);
    test_assert(len == 12); // quotes + 6 spaces + 0xff = 12
  } while (0);

  // Test 70: hexadecimal with prefix and zero padding
  sys_puts("Test 70: hexadecimal with prefix and zero padding\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#08x'", 255);
    test_cstrings_equal("'0x0000ff'", buffer);
    test_assert(len == 10); // quotes + 0x + 6 digits = 10
  } while (0);

  // Test 71: uppercase hexadecimal with prefix and width
  sys_puts("Test 71: uppercase hexadecimal with prefix and width\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#10X'", 255);
    test_cstrings_equal("'      0XFF'", buffer);
    test_assert(len == 12); // quotes + 6 spaces + 0XFF = 12
  } while (0);

  // Test 72: binary with prefix and width
  sys_puts("Test 72: binary with prefix and width\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#12b'", 15);
    test_cstrings_equal("'      0b1111'", buffer);
    test_assert(len == 14); // quotes + 6 spaces + 0b1111 = 14
  } while (0);

  // Test 73: binary with zero padding
  sys_puts("Test 73: binary with zero padding\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#010b'", 15);
    test_cstrings_equal("'0b00001111'", buffer);
    test_assert(len == 12); // quotes + 0b + 8 binary = 12
  } while (0);

  // Test 74: octal with prefix and width
  sys_puts("Test 74: octal with prefix and width\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#8o'", 64);
    test_cstrings_equal("'    0100'", buffer);
    test_assert(len == 10); // quotes + 4 spaces + 0100 = 10
  } while (0);

  // Test 75: large width value
  sys_puts("Test 75: large width value\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%20s'", "test");
    test_cstrings_equal("'                test'", buffer);
    test_assert(len == 22); // quotes + 16 spaces + 4 chars = 22
  } while (0);

  // Test 76: width with unsigned integers
  sys_puts("Test 76: width with unsigned integers\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08u'", 42U);
    test_cstrings_equal("'00000042'", buffer);
    test_assert(len == 10); // quotes + 8 digits = 10
  } while (0);

  // Test 77: width with long integers
  sys_puts("Test 77: width with long integers\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%10ld'", 123456L);
    test_cstrings_equal("'    123456'", buffer);
    test_assert(len == 12); // quotes + 4 spaces + 6 digits = 12
  } while (0);

  // Test 78: width with size_t
  sys_puts("Test 78: width with size_t\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08zd'", (size_t)999);
    test_cstrings_equal("'00000999'", buffer);
    test_assert(len == 10); // quotes + 8 digits = 10
  } while (0);

  // Test 79: width parsing with multiple format specifiers
  sys_puts("Test 79: width parsing with multiple format specifiers\n");
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%5d:%3s:%-4c", 12, "hi", 'x');
    test_cstrings_equal("   12: hi:x   ", buffer);
    test_assert(
        len ==
        14); // 3 spaces + 12 + colon + space + hi + colon + x + 3 spaces = 14
  } while (0);

  // Test 80: width with NULL string
  sys_puts("Test 80: width with NULL string\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%10s'", (char *)NULL);
    test_cstrings_equal("'     <nil>'", buffer);
    test_assert(len == 12); // quotes + 5 spaces + <nil> = 12
  } while (0);

  // Test 81: escaped percent with width (shouldn't affect %%)
  sys_puts("Test 81: escaped percent with width\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%%");
    test_cstrings_equal("%", buffer);
    test_assert(len == 1);
  } while (0);

  // Test 82: mixed width and non-width specifiers
  sys_puts("Test 82: mixed width and non-width specifiers\n");
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%d:%08x:%s", 1, 255, "end");
    test_cstrings_equal("1:000000ff:end", buffer);
    test_assert(len == 14);
  } while (0);

  // ===== END WIDTH SPECIFIER TESTS =====

  // Test 83: pointer format specifier
  sys_puts("Test 83: pointer format specifier\n");
  do {
    int var = 42;
    int *ptr = &var;
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%p", (void *)ptr);
    // Pointer should start with 0x and have hex digits
    test_assert(buffer[0] == '0' && buffer[1] == 'x');
    test_assert(len >= 3); // At least "0x" plus some hex digits
  } while (0);

  // Test 84: NULL pointer
  sys_puts("Test 84: NULL pointer\n");
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "null: %p", (void *)NULL);
    // Expected length: "null: 0x" + (2 * sizeof(void*)) hex digits
    size_t expected_len = 8 + (2 * sizeof(void *)); // "null: 0x" = 8 chars
    if (sizeof(void *) == 4) {
      test_cstrings_equal("null: 0x00000000", buffer);
    } else {
      test_cstrings_equal("null: 0x0000000000000000", buffer);
    }
    test_assert(len == expected_len);
  } while (0);

  // Test 85: pointer with other format specifiers
  sys_puts("Test 85: pointer with other format specifiers\n");
  do {
    int var = 123;
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%d:%p:%c", var, (void *)&var, 'P');
    // Should start with "123:0x" and end with ":P"
    test_assert(buffer[0] == '1' && buffer[1] == '2' && buffer[2] == '3' &&
                buffer[3] == ':');
    test_assert(buffer[4] == '0' && buffer[5] == 'x');
    test_assert(buffer[len - 2] == ':' && buffer[len - 1] == 'P');
    // Expected length: "123:0x" + (2 * sizeof(void*)) hex digits + ":P"
    size_t expected_len =
        8 + (2 * sizeof(void *)); // "123:0x" + hex digits + ":P"
    test_assert(len == expected_len);
  } while (0);

  // Test 86: specific pointer value
  sys_puts("Test 86: specific pointer value\n");
  do {
    char buffer[100];
    int dummy_var = 0;
    void *test_ptr = (void *)&dummy_var;
    size_t len = sys_sprintf(buffer, sizeof(buffer), "addr: %p", test_ptr);
    // Expected length: "addr: 0x" + (2 * sizeof(void*)) hex digits
    size_t expected_len = 8 + (2 * sizeof(void *)); // "addr: 0x" + hex digits
    test_assert(len == expected_len);
    test_assert(buffer[0] == 'a' && buffer[1] == 'd' && buffer[2] == 'd' &&
                buffer[3] == 'r' && buffer[4] == ':' && buffer[5] == ' ');
    test_assert(buffer[6] == '0' && buffer[7] == 'x');
  } while (0);

  // Test 87: pointer formatting consistency
  sys_puts("Test 87: pointer formatting consistency\n");
  do {
    size_t len = sys_printf("ptr: %p\n", (void *)0x1000);
    // Expected length calculation: "ptr: 0x00001000\n" = 16 chars
    // "ptr: " (5) + "0x00001000" (10) + "\n" (1) = 16 total
    // 32-bit: "ptr: 0x00001000\n" = 16 characters
    // 64-bit: "ptr: 0x0000000000001000\n" = 24 characters
    if (sizeof(void *) == 4) {
      test_assert(len == 16); // 32-bit: "ptr: 0x00001000\n" = 16 chars
    } else {
      test_assert(len == 24); // 64-bit: "ptr: 0x0000000000001000\n" = 24 chars
    }
  } while (0);

  // === BUFFER OVERFLOW TESTS ===

  // Test 88: exact buffer size (no null terminator space)
  sys_puts("Test 88: buffer overflow - exact size\n");
  do {
    char buffer[5];
    buffer[0] = 'X'; // Initialize to detect overwrites
    buffer[1] = 'X';
    buffer[2] = 'X';
    buffer[3] = 'X';
    buffer[4] = 'X';

    size_t len = sys_sprintf(buffer, 5, "Hello");
    test_assert(len == 5);          // Should return actual length needed
    test_assert(buffer[4] == '\0'); // Should null-terminate at last position
    test_cstrings_equal(buffer, "Hell"); // Should truncate to fit
  } while (0);

  // Test 89: buffer too small for string
  sys_puts("Test 89: buffer too small for string\n");
  do {
    char buffer[3];
    buffer[0] = 'X';
    buffer[1] = 'X';
    buffer[2] = 'X';

    size_t len = sys_sprintf(buffer, 3, "Hello World");
    test_assert(len == 11); // Should return full length that would be written
    test_assert(buffer[2] == '\0');    // Should null-terminate at last position
    test_cstrings_equal(buffer, "He"); // Should truncate to fit
  } while (0);

  // Test 90: single character buffer
  sys_puts("Test 90: single character buffer\n");
  do {
    char buffer[1];
    buffer[0] = 'X';

    size_t len = sys_sprintf(buffer, 1, "Hello");
    test_assert(len == 5);          // Should return full length
    test_assert(buffer[0] == '\0'); // Should only contain null terminator
  } while (0);

  // Test 91: zero-sized buffer
  sys_puts("Test 91: zero-sized buffer\n");
  do {
    char buffer[1];
    buffer[0] = 'X'; // Should remain unchanged

    size_t len = sys_sprintf(buffer, 0, "Hello");
    test_assert(len == 5);         // Should return full length
    test_assert(buffer[0] == 'X'); // Buffer should be unchanged when size=0
  } while (0);

  // Test 92: NULL buffer with zero size
  sys_puts("Test 92: NULL buffer with zero size\n");
  do {
    size_t len = sys_sprintf(NULL, 0, "Hello World");
    test_assert(len == 11); // Should return full length
  } while (0);

  // Test 93: buffer overflow with format specifiers
  sys_puts("Test 93: buffer overflow with format specifiers\n");
  do {
    char buffer[10];
    for (int i = 0; i < 10; i++)
      buffer[i] = 'X'; // Initialize

    size_t len =
        sys_sprintf(buffer, 10, "Number: %d, String: %s", 12345, "Hello");
    test_assert(len == 28);         // "Number: 12345, String: Hello" = 28 chars
    test_assert(buffer[9] == '\0'); // Should null-terminate at last position
    test_cstrings_equal(buffer, "Number: 1"); // Should truncate
  } while (0);

  // Test 94: width specifier overflow
  sys_puts("Test 94: width specifier overflow\n");
  do {
    char buffer[8];
    for (int i = 0; i < 8; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 8, "%20s", "Hi");
    test_assert(len == 20);                 // Should return full padded length
    test_assert(buffer[7] == '\0');         // Should null-terminate
    test_cstrings_equal(buffer, "       "); // Should contain spaces (truncated)
  } while (0);

  // Test 95: zero-padded number overflow
  sys_puts("Test 95: zero-padded number overflow\n");
  do {
    char buffer[5];
    for (int i = 0; i < 5; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 5, "%08d", 42);
    test_assert(len == 8);               // "00000042" = 8 chars
    test_assert(buffer[4] == '\0');      // Should null-terminate
    test_cstrings_equal(buffer, "0000"); // Should truncate
  } while (0);

  // Test 96: pointer formatting overflow
  sys_puts("Test 96: pointer formatting overflow\n");
  do {
    char buffer[10];
    for (int i = 0; i < 10; i++)
      buffer[i] = 'X'; // Initialize

    int dummy_var = 0; // Local variable to obtain a valid memory address
    size_t len = sys_sprintf(buffer, 10, "%p", (void *)&dummy_var);
    // Expected length: "0x" + (2 * sizeof(void*)) hex digits
    size_t expected_len = 2 + (2 * sizeof(void *));
    test_assert(len == expected_len);
    test_assert(buffer[9] == '\0'); // Should null-terminate
    // Should start with "0x"
    test_assert(buffer[0] == '0' && buffer[1] == 'x');
    test_assert(buffer[2] >= '0' && buffer[2] <= 'f'); // Valid hex digit
  } while (0);

  // Test 97: multiple format specifiers with small buffer
  sys_puts("Test 97: multiple format specifiers with small buffer\n");
  do {
    char buffer[6];
    for (int i = 0; i < 6; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 6, "%d+%d=%d", 123, 456, 579);
    test_assert(len == 11);               // "123+456=579" = 11 chars
    test_assert(buffer[5] == '\0');       // Should null-terminate
    test_cstrings_equal(buffer, "123+4"); // Should truncate
  } while (0);

  // Test 98: long string with NULL
  sys_puts("Test 98: long string with NULL\n");
  do {
    char buffer[8];
    for (int i = 0; i < 8; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 8, "Hello %s World", (char *)NULL);
    test_assert(len == 17);                 // "Hello <nil> World" = 17 chars
    test_assert(buffer[7] == '\0');         // Should null-terminate
    test_cstrings_equal(buffer, "Hello <"); // Should truncate
  } while (0);

  // Test 99: character with width overflow
  sys_puts("Test 99: character with width overflow\n");
  do {
    char buffer[4];
    for (int i = 0; i < 4; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 4, "%10c", 'A');
    test_assert(len == 10);             // 9 spaces + 'A' = 10 chars
    test_assert(buffer[3] == '\0');     // Should null-terminate
    test_cstrings_equal(buffer, "   "); // Should be spaces (truncated)
  } while (0);

  // === TEST sys_vsprintf NULL TERMINATION ===

  // Test 100: sys_vsprintf with buffer overflow
  sys_puts("Test 100: sys_vsprintf with buffer overflow\n");
  do {
    char buffer[8];
    for (int i = 0; i < 8; i++)
      buffer[i] = 'X'; // Initialize to detect overwrites

    size_t len =
        helper_test_sys_vsprintf(buffer, 8, "Hello %s World", "Beautiful");
    test_assert(len == 21);         // "Hello Beautiful World" = 21 chars
    test_assert(buffer[7] == '\0'); // Should null-terminate at last position
    test_cstrings_equal(buffer, "Hello B"); // Should truncate correctly
  } while (0);

  // Test 101: sys_vsprintf with exact buffer size
  sys_puts("Test 101: sys_vsprintf with exact buffer size\n");
  do {
    char buffer[6];
    for (int i = 0; i < 6; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = helper_test_sys_vsprintf(buffer, 6, "Test%d", 42);
    test_assert(len == 6);          // "Test42" = 6 chars
    test_assert(buffer[5] == '\0'); // Should null-terminate at last position
    test_cstrings_equal(buffer, "Test4"); // Should truncate to fit
  } while (0);

  // Test 102: sys_vsprintf with empty buffer
  sys_puts("Test 102: sys_vsprintf with empty buffer\n");
  do {
    char buffer[1];
    buffer[0] = 'X'; // Initialize

    size_t len = helper_test_sys_vsprintf(buffer, 1, "Hello");
    test_assert(len == 5);          // Should return full length needed
    test_assert(buffer[0] == '\0'); // Should null-terminate empty buffer
  } while (0);

  // All tests completed successfully
  sys_puts("All 102 tests completed successfully!\n");

  // Return success
  return 0;
}
