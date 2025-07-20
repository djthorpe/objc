#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

int main() {
  const int bits = NXArchBits();
  test_assert(bits == 64 || bits == 32);
  NXLog(@"Architecture bits: %d", bits);

  const NXEndian endian = NXArchEndian();
  test_assert(endian == NXBigEndian || endian == NXLittleEndian);
  NXLog(@"Architecture endian: %s", endian == NXBigEndian ? "big" : "little");

  return 0;
}
