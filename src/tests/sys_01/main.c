#include <sys/sys.h>
#include <tests/tests.h>

int main(void) {
  // Output a message to the console
  do {
    size_t len = sys_printf("Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // Output a formatted message
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // ...with no buffer
  do {
    size_t len = sys_sprintf(NULL, 0, "Hello, World!\n");
    test_assert(len == 14);
  } while (0);

  // ...with % at the end, just print the %
  do {
    size_t len = sys_printf("Hello, World!%");
    test_assert(len == 14);
  } while (0);

  // ...with %% at the end
  do {
    size_t len = sys_printf("Hello, World!%%");
    test_assert(len == 14);
  } while (0);

  // ...with %% in the middle
  do {
    size_t len = sys_printf("Hello%% World!\n");
    test_assert(len == 14);
  } while (0);

  // ...with %s and zero-length c-string
  do {
    size_t len = sys_printf("Hello%s, World!\n", "");
    test_assert(len == 14);
  } while (0);

  // ...with %s and c-string
  do {
    size_t len = sys_printf("Hello, %s\n", "World!");
    test_assert(len == 14);
  } while (0);

  // ...with %s and NULL c-string
  do {
    size_t len = sys_printf("Hello%s World!\n", NULL);
    test_assert(len == 18);
  } while (0);

  // ...with missing argument
  do {
    size_t len = sys_printf("Hello%s World!\n");
    test_assert(len == 18);
  } while (0);

  // ...with a decimal
  do {
    size_t len = sys_printf("42==%d\n", 42);
    test_assert(len == 7);
  } while (0);

  // ...with a negative decimal
  do {
    size_t len = sys_printf("-42==%d\n", -42);
    test_assert(len == 9);
  } while (0);

  // ...with a positive decimal
  do {
    size_t len = sys_printf("+42==%+d\n", 42);
    test_assert(len == 9); // "+42==+42\n" = 9 characters
  } while (0);

  // ...with a negative decimal
  do {
    size_t len = sys_printf("-42==%+d\n", -42);
    test_assert(len == 9); // "-42==-42\n" = 9 characters
  } while (0);

  // ...with a decimal
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "42==%d", 42);
    test_cstrings_equal("42==42", buffer);
    test_assert(len == 6);
  } while (0);

  // Test hexadecimal format specifiers
  do {
    size_t len = sys_printf("255==%x\n", 255);
    test_assert(len == 8); // "255==ff\n" = 8 characters
  } while (0);

  do {
    size_t len = sys_printf("255==%X\n", 255);
    test_assert(len == 8); // "255==FF\n" = 8 characters
  } while (0);

  // Test hexadecimal with prefix
  do {
    size_t len = sys_printf("255==%#x\n", 255);
    test_assert(len == 10); // "255==0xff\n" = 10 characters
  } while (0);

  do {
    size_t len = sys_printf("255==%#X\n", 255);
    test_assert(len == 10); // "255==0XFF\n" = 10 characters
  } while (0);

  // Test binary format specifier
  do {
    size_t len = sys_printf("15==%b\n", 15);
    test_assert(len == 9); // "15==1111\n" = 9 characters
  } while (0);

  // Test binary with prefix
  do {
    size_t len = sys_printf("15==%#b\n", 15);
    test_assert(len == 11); // "15==0b1111\n" = 11 characters
  } while (0);

  // Test octal format specifier
  do {
    size_t len = sys_printf("64==%o\n", 64);
    test_assert(len == 8); // "64==100\n" = 8 characters
  } while (0);

  // Test octal with prefix
  do {
    size_t len = sys_printf("64==%#o\n", 64);
    test_assert(len == 9); // "64==0100\n" = 9 characters
  } while (0);

  // Test unsigned decimal
  do {
    size_t len = sys_printf("4294967295==%u\n", 4294967295U);
    test_assert(len == 23); // "4294967295==4294967295\n" = 23 characters
  } while (0);

  // Test character format specifier
  do {
    size_t len = sys_printf("A==%c\n", 'A');
    test_assert(len == 5); // "A==A\n" = 5 characters
  } while (0);

  // Test more character format specifiers
  do {
    size_t len = sys_printf("char: %c %c %c\n", 'X', 'Y', 'Z');
    test_assert(len == 12); // "char: X Y Z\n" = 12 characters
  } while (0);

  // Test special characters
  do {
    size_t len = sys_printf("special: %c%c%c\n", ' ', '\t', '!');
    test_assert(
        len ==
        13); // "special:  	!\n" = 13 characters (space, tab, exclamation)
  } while (0);

  // Test numeric characters
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%c%c%c", '0', '5', '9');
    test_cstrings_equal("059", buffer);
    test_assert(len == 3);
  } while (0);

  // Test character with sprintf
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Letter: %c", 'Q');
    test_cstrings_equal("Letter: Q", buffer);
    test_assert(len == 9);
  } while (0);

  // Test ASCII control characters (printable range)
  do {
    size_t len =
        sys_printf("ASCII: %c %c %c\n", 32, 65, 126); // space, 'A', '~'
    test_assert(len == 13); // "ASCII:   A ~\n" = 13 characters
  } while (0);

  // Test character promotion from int
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%c%c", (int)'H', (int)'i');
    test_cstrings_equal("Hi", buffer);
    test_assert(len == 2);
  } while (0);

  // Test punctuation and symbols
  do {
    size_t len = sys_printf("symbols: %c%c%c%c%c\n", '@', '#', '$', '%', '&');
    test_assert(len == 15); // "symbols: @#$%&\n" = 15 characters
  } while (0);

  // Test mixed characters and other format specifiers
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "Mix: %d%c%s%c%d", 1, ':',
                             "test", ':', 2);
    test_cstrings_equal("Mix: 1:test:2", buffer);
    test_assert(len == 13);
  } while (0);

  // Test character with zero value (null character)
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "null%cend", 0);
    test_assert(
        len == 8); // "null\0end" has length 8 but null terminates at position 4
    // Note: buffer will be "null" due to null termination, but printf counted
    // all characters
  } while (0);

  // Test long integers
  do {
    size_t len = sys_printf("123456789==%ld\n", 123456789L);
    test_assert(len == 21); // "123456789==123456789\n" = 21 characters
  } while (0);

  // Test long unsigned integers
  do {
    size_t len = sys_printf("123456789==%lu\n", 123456789UL);
    test_assert(len == 21); // "123456789==123456789\n" = 21 characters
  } while (0);

  // Test size_t integers
  do {
    size_t len = sys_printf("12345==%zd\n", (size_t)12345);
    test_assert(len == 13); // "12345==12345\n" = 13 characters
  } while (0);

  // Test size_t unsigned integers
  do {
    size_t len = sys_printf("12345==%zu\n", (size_t)12345);
    test_assert(len == 13); // "12345==12345\n" = 13 characters
  } while (0);

  // Test sprintf with hexadecimal
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "0x%x", 255);
    test_cstrings_equal("0xff", buffer);
    test_assert(len == 4);
  } while (0);

  // Test sprintf with binary
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%#b", 7);
    test_cstrings_equal("0b111", buffer);
    test_assert(len == 5);
  } while (0);

  // Test sprintf with octal
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%#o", 8);
    test_cstrings_equal("010", buffer);
    test_assert(len == 3);
  } while (0);

  // Test edge case: zero with different formats
  do {
    size_t len = sys_printf("0: dec=%d hex=%x bin=%b oct=%o\n", 0, 0, 0, 0);
    test_assert(len == 27); // "0: dec=0 hex=0 bin=0 oct=0\n" = 27 characters
  } while (0);

  // Test edge case: maximum values
  do {
    size_t len = sys_printf("max int: %d\n", 2147483647);
    test_assert(len == 20); // "max int: 2147483647\n" = 20 characters
  } while (0);

  // Test mixed format specifiers in one call
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer),
                    "Dec:%d Hex:%x Bin:%b Oct:%o Char:%c", 42, 42, 42, 42, 'Z');
    test_cstrings_equal("Dec:42 Hex:2a Bin:101010 Oct:52 Char:Z", buffer);
    test_assert(len == 38);
  } while (0);

  // Test negative values
  do {
    size_t len = sys_printf("negative: %d\n", -123);
    test_assert(len == 15); // "negative: -123\n" = 15 characters
  } while (0);

  // Test negative with positive sign flag
  do {
    size_t len = sys_printf("negative: %+d\n", -456);
    test_assert(len == 15); // "negative: -456\n" = 15 characters
  } while (0);

  // Test negative long integers
  do {
    size_t len = sys_printf("negative long: %ld\n", -9876543210L);
    test_assert(len == 27); // "negative long: -9876543210\n" = 27 characters
  } while (0);

  // Test negative with sprintf
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "neg:%d pos:%d", -42, 42);
    test_cstrings_equal("neg:-42 pos:42", buffer);
    test_assert(len == 14);
  } while (0);

  // Test very negative numbers (near INT_MIN)
  do {
    size_t len = sys_printf("min int: %d\n", -2147483647);
    test_assert(len == 21); // "min int: -2147483647\n" = 21 characters
  } while (0);

  // Test INT_MIN edge case (most negative 32-bit integer)
  do {
    size_t len = sys_printf("int min: %d\n", (-2147483647 - 1));
    test_assert(len == 21); // "int min: -2147483648\n" = 21 characters
  } while (0);

  // Test negative long minimum (LONG_MIN edge case)
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%ld", (-9223372036854775807L - 1));
    test_cstrings_equal("-9223372036854775808", buffer);
    test_assert(len == 20); // "-9223372036854775808" = 20 characters
  } while (0);

  // Test multiple negative values in one call
  do {
    size_t len = sys_printf("a:%d b:%d c:%d\n", -1, -22, -333);
    test_assert(len == 18); // "a:-1 b:-22 c:-333\n" = 18 characters
  } while (0);

  // Test zero vs negative zero
  do {
    size_t len = sys_printf("zero: %d, neg-zero: %d\n", 0, -0);
    test_assert(len == 21); // "zero: 0, neg-zero: 0\n" = 21 characters
  } while (0);

  // Test negative with positive sign flag
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "neg:%+d zero:%+d pos:%+d",
                             -5, 0, 5);
    test_cstrings_equal("neg:-5 zero:+0 pos:+5", buffer);
    test_assert(len == 21);
  } while (0);

  // Test that unsigned formats don't make sense with negative (but should work
  // as wraparound)
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "unsigned -1: %u", -1);
    test_cstrings_equal("unsigned -1: 4294967295",
                        buffer); // Two's complement wraparound
    test_assert(len == 23);
  } while (0);

  // Test negative values are not used with hex/binary/octal (they use unsigned)
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "hex -1: %x", -1);
    test_cstrings_equal("hex -1: ffffffff",
                        buffer); // Two's complement representation
    test_assert(len == 16);
  } while (0);

  // ===== WIDTH SPECIFIER TESTS =====

  // Test string width - right aligned (default)
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%10s'", "hello");
    test_cstrings_equal("'     hello'", buffer);
    test_assert(len == 12); // quotes + 5 spaces + 5 chars = 12
  } while (0);

  // Test string width - left aligned
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%-10s'", "hello");
    test_cstrings_equal("'hello     '", buffer);
    test_assert(len == 12); // quotes + 5 chars + 5 spaces = 12
  } while (0);

  // Test string width - exact fit
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%5s'", "hello");
    test_cstrings_equal("'hello'", buffer);
    test_assert(len == 7); // quotes + 5 chars = 7
  } while (0);

  // Test string width - smaller than string (no truncation)
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%3s'", "hello");
    test_cstrings_equal("'hello'", buffer);
    test_assert(len == 7); // quotes + 5 chars = 7
  } while (0);

  // Test character width - right aligned
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%5c'", 'A');
    test_cstrings_equal("'    A'", buffer);
    test_assert(len == 7); // quotes + 4 spaces + 1 char = 7
  } while (0);

  // Test character width - left aligned
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%-5c'", 'A');
    test_cstrings_equal("'A    '", buffer);
    test_assert(len == 7); // quotes + 1 char + 4 spaces = 7
  } while (0);

  // Test integer width - right aligned with spaces
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%8d'", 42);
    test_cstrings_equal("'      42'", buffer);
    test_assert(len == 10); // quotes + 6 spaces + 2 digits = 10
  } while (0);

  // Test integer width - left aligned
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%-8d'", 42);
    test_cstrings_equal("'42      '", buffer);
    test_assert(len == 10); // quotes + 2 digits + 6 spaces = 10
  } while (0);

  // Test integer width - zero padded
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08d'", 42);
    test_cstrings_equal("'00000042'", buffer);
    test_assert(len == 10); // quotes + 8 digits = 10
  } while (0);

  // Test negative integer with zero padding
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08d'", -42);
    test_cstrings_equal("'-0000042'", buffer);
    test_assert(len == 10); // quotes + sign + 7 digits = 10
  } while (0);

  // Test positive sign with width and zero padding
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%+08d'", 42);
    test_cstrings_equal("'+0000042'", buffer);
    test_assert(len == 10); // quotes + sign + 7 digits = 10
  } while (0);

  // Test hexadecimal width - space padded
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%8x'", 255);
    test_cstrings_equal("'      ff'", buffer);
    test_assert(len == 10); // quotes + 6 spaces + 2 hex = 10
  } while (0);

  // Test hexadecimal width - zero padded
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08x'", 255);
    test_cstrings_equal("'000000ff'", buffer);
    test_assert(len == 10); // quotes + 8 hex = 10
  } while (0);

  // Test hexadecimal with prefix and width
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#10x'", 255);
    test_cstrings_equal("'      0xff'", buffer);
    test_assert(len == 12); // quotes + 6 spaces + 0xff = 12
  } while (0);

  // Test hexadecimal with prefix and zero padding
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#08x'", 255);
    test_cstrings_equal("'0x0000ff'", buffer);
    test_assert(len == 10); // quotes + 0x + 6 digits = 10
  } while (0);

  // Test uppercase hexadecimal with prefix and width
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#10X'", 255);
    test_cstrings_equal("'      0XFF'", buffer);
    test_assert(len == 12); // quotes + 6 spaces + 0XFF = 12
  } while (0);

  // Test binary with prefix and width
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#12b'", 15);
    test_cstrings_equal("'      0b1111'", buffer);
    test_assert(len == 14); // quotes + 6 spaces + 0b1111 = 14
  } while (0);

  // Test binary with zero padding
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#010b'", 15);
    test_cstrings_equal("'0b00001111'", buffer);
    test_assert(len == 12); // quotes + 0b + 8 binary = 12
  } while (0);

  // Test octal with prefix and width
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%#8o'", 64);
    test_cstrings_equal("'    0100'", buffer);
    test_assert(len == 10); // quotes + 4 spaces + 0100 = 10
  } while (0);

  // Test large width value
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%20s'", "test");
    test_cstrings_equal("'                test'", buffer);
    test_assert(len == 22); // quotes + 16 spaces + 4 chars = 22
  } while (0);

  // Test width with unsigned integers
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08u'", 42U);
    test_cstrings_equal("'00000042'", buffer);
    test_assert(len == 10); // quotes + 8 digits = 10
  } while (0);

  // Test width with long integers
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%10ld'", 123456L);
    test_cstrings_equal("'    123456'", buffer);
    test_assert(len == 12); // quotes + 4 spaces + 6 digits = 12
  } while (0);

  // Test width with size_t
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%08zd'", (size_t)999);
    test_cstrings_equal("'00000999'", buffer);
    test_assert(len == 10); // quotes + 8 digits = 10
  } while (0);

  // Test width parsing with multiple format specifiers
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%5d:%3s:%-4c", 12, "hi", 'x');
    test_cstrings_equal("   12: hi:x   ", buffer);
    test_assert(
        len ==
        14); // 3 spaces + 12 + colon + space + hi + colon + x + 3 spaces = 14
  } while (0);

  // Test width with NULL string
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "'%10s'", (char *)NULL);
    test_cstrings_equal("'     <nil>'", buffer);
    test_assert(len == 12); // quotes + 5 spaces + <nil> = 12
  } while (0);

  // Test that width doesn't affect %% (escaped percent)
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%%");
    test_cstrings_equal("%", buffer);
    test_assert(len == 1);
  } while (0);

  // Test mixed width and non-width specifiers
  do {
    char buffer[100];
    size_t len =
        sys_sprintf(buffer, sizeof(buffer), "%d:%08x:%s", 1, 255, "end");
    test_cstrings_equal("1:000000ff:end", buffer);
    test_assert(len == 14);
  } while (0);

  // ===== END WIDTH SPECIFIER TESTS =====

  // Test pointer format specifier
  do {
    int var = 42;
    int *ptr = &var;
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "%p", (void *)ptr);
    // Pointer should start with 0x and have hex digits
    test_assert(buffer[0] == '0' && buffer[1] == 'x');
    test_assert(len >= 3); // At least "0x" plus some hex digits
  } while (0);

  // Test NULL pointer
  do {
    char buffer[100];
    size_t len = sys_sprintf(buffer, sizeof(buffer), "null: %p", (void *)NULL);
    test_cstrings_equal("null: 0x0000000000000000", buffer);
    test_assert(len == 24); // "null: 0x" + 16 zeros = 24 characters
  } while (0);

  // Test pointer with other format specifiers
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
    test_assert(len == 24); // "123:0x" + 16 hex digits + ":P" = 24 characters
  } while (0);

  // Test specific pointer value
  do {
    char buffer[100];
    int dummy_var = 0;
    void *test_ptr = (void *)&dummy_var;
    size_t len = sys_sprintf(buffer, sizeof(buffer), "addr: %p", test_ptr);
    test_assert(len >= 10); // "addr: 0x" + at least some hex digits
  } while (0);

  // Test pointer formatting consistency
  do {
    size_t len = sys_printf("ptr: %p\n", (void *)0x1000);
    test_assert(len == 24); // "ptr: 0x" + 16 hex digits + "\n" = 24 characters
  } while (0);

  // === BUFFER OVERFLOW TESTS ===

  // Test 1: Exact buffer size (no null terminator space)
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

  // Test 2: Buffer too small for string
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

  // Test 3: Single character buffer
  do {
    char buffer[1];
    buffer[0] = 'X';

    size_t len = sys_sprintf(buffer, 1, "Hello");
    test_assert(len == 5);          // Should return full length
    test_assert(buffer[0] == '\0'); // Should only contain null terminator
  } while (0);

  // Test 4: Zero-sized buffer
  do {
    char buffer[1];
    buffer[0] = 'X'; // Should remain unchanged

    size_t len = sys_sprintf(buffer, 0, "Hello");
    test_assert(len == 5);         // Should return full length
    test_assert(buffer[0] == 'X'); // Buffer should be unchanged when size=0
  } while (0);

  // Test 5: NULL buffer with zero size (should not crash)
  do {
    size_t len = sys_sprintf(NULL, 0, "Hello World");
    test_assert(len == 11); // Should return full length
  } while (0);

  // Test 6: Buffer overflow with format specifiers
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

  // Test 7: Width specifier overflow
  do {
    char buffer[8];
    for (int i = 0; i < 8; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 8, "%20s", "Hi");
    test_assert(len == 20);                 // Should return full padded length
    test_assert(buffer[7] == '\0');         // Should null-terminate
    test_cstrings_equal(buffer, "       "); // Should contain spaces (truncated)
  } while (0);

  // Test 8: Zero-padded number overflow
  do {
    char buffer[5];
    for (int i = 0; i < 5; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 5, "%08d", 42);
    test_assert(len == 8);               // "00000042" = 8 chars
    test_assert(buffer[4] == '\0');      // Should null-terminate
    test_cstrings_equal(buffer, "0000"); // Should truncate
  } while (0);

  // Test 9: Pointer formatting overflow
  do {
    char buffer[10];
    for (int i = 0; i < 10; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 10, "%p", (void *)0x123456789ABCDEF0UL);
    test_assert(len == 18);                   // "0x" + 16 hex digits = 18 chars
    test_assert(buffer[9] == '\0');           // Should null-terminate
    test_cstrings_equal(buffer, "0x1234567"); // Should truncate hex digits
  } while (0);

  // Test 10: Multiple format specifiers with small buffer
  do {
    char buffer[6];
    for (int i = 0; i < 6; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 6, "%d+%d=%d", 123, 456, 579);
    test_assert(len == 11);               // "123+456=579" = 11 chars
    test_assert(buffer[5] == '\0');       // Should null-terminate
    test_cstrings_equal(buffer, "123+4"); // Should truncate
  } while (0);

  // Test 11: Long string with NULL
  do {
    char buffer[8];
    for (int i = 0; i < 8; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 8, "Hello %s World", (char *)NULL);
    test_assert(len == 17);                 // "Hello <nil> World" = 17 chars
    test_assert(buffer[7] == '\0');         // Should null-terminate
    test_cstrings_equal(buffer, "Hello <"); // Should truncate
  } while (0);

  // Test 12: Character with width overflow
  do {
    char buffer[4];
    for (int i = 0; i < 4; i++)
      buffer[i] = 'X'; // Initialize

    size_t len = sys_sprintf(buffer, 4, "%10c", 'A');
    test_assert(len == 10);             // 9 spaces + 'A' = 10 chars
    test_assert(buffer[3] == '\0');     // Should null-terminate
    test_cstrings_equal(buffer, "   "); // Should be spaces (truncated)
  } while (0);

  sys_printf("All tests passed!\n");
}
