
#include "runtime-objc.h"
#include <tests/tests.h>

int main(void) {
  sys_init();

  test_runtime_01();

  sys_exit();
  return 0;
}
