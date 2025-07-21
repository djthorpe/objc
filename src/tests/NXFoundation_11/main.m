#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

int main() {
  // Allocate a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);

  const int bits = NXArchBits();
  test_assert(bits == 64 || bits == 32);
  NXLog(@"Architecture bits: %d", bits);

  const NXEndian endian = NXArchEndian();
  test_assert(endian == NXBigEndian || endian == NXLittleEndian);
  NXLog(@"Architecture endian: %s", endian == NXBigEndian ? "big" : "little");

  // Free the zone
  [zone release];
  test_assert([NXZone defaultZone] == nil);

  return 0;
}
