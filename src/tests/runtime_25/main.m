#include <objc/objc.h>
#include <string.h>
#include <tests/tests.h>

int main() {
  const char *str1 = "str1=[Running printf tests...]\n";
  const char *str2 = "str2=%\n";
  const char *str3 = "str3=%%\n";
  const char *str4 = "str4=[%%%s]\n";
  //  const char *str4 = "%s%%";
  //  const char *str5 = "%%%s%%";

  objc_printf(str1);
  objc_printf(str2);
  objc_printf(str3);
  objc_printf(str4, str1);

  /*
    do {
      size_t len = objc_sprintf(NULL, 0, str1);
      test_assert(strlen(str1) == len);
    } while (0);

    do {
      char buf[80];
      size_t len = objc_sprintf(buf, 80, str1);
      test_assert(strlen(str1) == len);
      test_assert(strcmp(buf, str1) == 0);
    } while (0);

    do {
      char buf[80];
      size_t len = objc_sprintf(buf, 80, str2);
      test_assert(strlen(str2) == len);
      test_assert(strcmp(buf, str2) == 0);
    } while (0);

    do {
      char buf[80];
      size_t len = objc_sprintf(buf, 80, str3, str1);
      test_assert(strlen(str1) + 1 == len);
    } while (0);

    do {
      char buf[80];
      size_t len = objc_sprintf(buf, 80, str4, str1);
      test_assert(strlen(str1) + 1 == len);
    } while (0);

    do {
      char buf[80];
      size_t len = objc_sprintf(buf, 80, str5, str1);
      test_assert(strlen(str1) + 2 == len);
    } while (0);
  */

  // Return success
  return 0;
}
