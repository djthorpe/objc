#include <NXFoundation/NXFoundation.h>
#include <tests/tests.h>

int main() {
  // Allocate a zone
  NXZone *zone = [NXZone zoneWithSize:1024];
  test_assert(zone != nil);

  // Test NXArchBits
  const int bits = NXArchBits();
  test_assert(bits == 64 || bits == 32);
  NXLog(@"Architecture bits: %d", bits);

  // Test consistency - multiple calls should return same value
  const int bits2 = NXArchBits();
  test_assert(bits == bits2);
  NXLog(@"Architecture bits consistency test passed");

  // Test NXArchEndian
  const NXEndian endian = NXArchEndian();
  test_assert(endian == NXBigEndian || endian == NXLittleEndian);
  NXLog(@"Architecture endian: %s", endian == NXBigEndian ? "big" : "little");

  // Test endian consistency
  const NXEndian endian2 = NXArchEndian();
  test_assert(endian == endian2);
  NXLog(@"Architecture endian consistency test passed");

  // Test NXArchNumCores
  const uint8_t cores = NXArchNumCores();
  test_assert(cores >= 1);
  test_assert(cores <= 255); // Should not exceed uint8_t max
  NXLog(@"Architecture cores: %u", cores);

  // Test cores consistency
  const uint8_t cores2 = NXArchNumCores();
  test_assert(cores == cores2);
  NXLog(@"Architecture cores consistency test passed");

  // Test NXArchOS
  const char *os = NXArchOS();
  test_assert(os != nil);
  test_assert(strlen(os) > 0);
  NXLog(@"Architecture OS: %s", os);

  // Test OS consistency
  const char *os2 = NXArchOS();
  test_assert(os == os2); // Should be same pointer (compile-time constant)
  test_assert(strcmp(os, os2) == 0);
  NXLog(@"Architecture OS consistency test passed");

  // Verify OS is one of expected values
  test_assert(strcmp(os, "darwin") == 0 || strcmp(os, "linux") == 0 ||
              strcmp(os, "pico") == 0);
  NXLog(@"Architecture OS validation test passed");

  // Test NXArchProcessor
  const char *processor = NXArchProcessor();
  test_assert(processor != nil);
  test_assert(strlen(processor) > 0);
  NXLog(@"Architecture processor: %s", processor);

  // Test processor consistency
  const char *processor2 = NXArchProcessor();
  test_assert(processor ==
              processor2); // Should be same pointer (compile-time constant)
  test_assert(strcmp(processor, processor2) == 0);
  NXLog(@"Architecture processor consistency test passed");

  // Verify processor is one of expected values
  test_assert(strcmp(processor, "arm64") == 0 ||
              strcmp(processor, "x86-64") == 0 ||
              strcmp(processor, "aarch64") == 0 ||
              strcmp(processor, "cortex-m0plus") == 0);
  NXLog(@"Architecture processor validation test passed");

  // Test cross-function consistency
  // On 64-bit systems, we expect certain processor types
  if (bits == 64) {
    test_assert(strcmp(processor, "arm64") == 0 ||
                strcmp(processor, "x86-64") == 0 ||
                strcmp(processor, "aarch64") == 0);
    NXLog(@"64-bit architecture consistency test passed");
  }

  // Test that endianness makes sense for known processors
  if (strcmp(processor, "arm64") == 0 || strcmp(processor, "aarch64") == 0 ||
      strcmp(processor, "x86-64") == 0 ||
      strcmp(processor, "cortex-m0plus") == 0) {
    test_assert(endian == NXLittleEndian);
    NXLog(@"Processor-endian consistency test passed");
  }

  // Free the zone
  [zone release];
  test_assert([NXZone defaultZone] == nil);

  return 0;
}
