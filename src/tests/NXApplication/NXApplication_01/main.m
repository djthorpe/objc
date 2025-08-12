#include <NXApplication/NXApplication.h>
#include <tests/tests.h>

#ifdef SYSTEM_NAME_PICO
// HACK
void *stdout = NULL;
void *stderr = NULL;
#endif

int test_NXApplication_01(void) {
  // Check if the GPIO module is available
  if ([GPIO count] == 0) {
    NXLog(@"No GPIO support available, skipping test.");
    return 0; // No GPIO pins available
  }

  // Create a GPIO input instance
  GPIO *gpioInput = [GPIO inputWithPin:12];
  objc_assert(gpioInput);
  NXLog(@"GPIO: %p state: %s", gpioInput, [gpioInput state] ? "HIGH" : "LOW");

  return 0;
}

int main(void) {
  return TestAppMain("test_NXApplication_01", test_NXApplication_01);
}
