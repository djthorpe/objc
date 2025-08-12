
#include "runtime-sys.h"
#include <tests/tests.h>

int main(void) {
  sys_init();

  int return_code = 0;
  return_code |= test_sys_00();
  return_code |= test_sys_01();
  // FIX test_sys_02();
  return_code |= test_sys_03();
  return_code |= test_sys_04();
  return_code |= test_sys_05();
  return_code |= test_sys_06();
  return_code |= test_sys_07();
  return_code |= test_sys_08();
  // FIX test_sys_09();
  return_code |= test_sys_10();
  return_code |= test_sys_11();
  return_code |= test_sys_12();
  return_code |= test_sys_13();
  // FIX test_sys_14();
  return_code |= test_sys_15();
  return_code |= test_sys_16();
  return_code |= test_sys_17();

  // End tests
  if (return_code == 0) {
    sys_printf("ALL TESTS PASSED\n");
  } else {
    sys_printf("SOME TESTS FAILED\n");
  }

  sys_exit();
  return 0;
}
