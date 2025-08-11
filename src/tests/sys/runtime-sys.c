
#include "runtime-sys.h"
#include <tests/tests.h>

int main(void) {
  sys_init();

  test_sys_00();
  test_sys_01();
  // FIX test_sys_02();
  test_sys_03();
  test_sys_04();
  test_sys_05();
  test_sys_06();
  test_sys_07();
  test_sys_08();
  // FIX test_sys_09();
  test_sys_10();
  test_sys_11();
  test_sys_12();
  test_sys_13();
  // FIX test_sys_14();
  test_sys_15();
  test_sys_16();

  sys_exit();
  return 0;
}
