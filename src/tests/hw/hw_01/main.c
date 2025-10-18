#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

struct mode_description_t {
  hw_gpio_mode_t mode;     ///< The GPIO mode
  const char *description; ///< Description of the mode
};

static const struct mode_description_t mode_descriptions[] = {
    {HW_GPIO_INPUT, "Floating input"},
    {HW_GPIO_PULLUP, "Input with pull-up resistor"},
    {HW_GPIO_PULLDOWN, "Input with pull-down resistor"},
    {HW_GPIO_OUTPUT, "Output"},
    {HW_GPIO_SPI, "SPI"},
    {HW_GPIO_I2C, "I2C"},
    {HW_GPIO_UART, "UART"},
    {HW_GPIO_PWM, "PWM"},
    {HW_GPIO_ADC, "ADC"},
    {HW_GPIO_UNKNOWN, "Unknown or unsupported mode"}};

static const char *get_mode_description(hw_gpio_mode_t mode) {
  for (size_t i = 0;
       i < sizeof(mode_descriptions) / sizeof(mode_descriptions[0]); i++) {
    if (mode_descriptions[i].mode == mode) {
      return mode_descriptions[i].description;
    }
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_hw_01(void) {
  uint8_t bank = 0;
  uint8_t pin_count = hw_gpio_count(bank);
  sys_printf("GPIO Bank %d: %d pins\n", bank, pin_count);
  if (pin_count == 0) {
    sys_printf("  No GPIO pins available in this bank.\n");
    return 0;
  }

  uint8_t pin = 0;
  for (pin = 0; pin < pin_count; pin++) {
    hw_gpio_t gpio = hw_gpio_init(bank, pin, HW_GPIO_NONE);
    if (hw_gpio_valid(&gpio) == false) {
      sys_printf("  GPIO pin %02d: invalid\n", pin);
      continue;
    }
    hw_gpio_mode_t mode = hw_gpio_get_mode(&gpio);
    sys_printf("GPIO pin %02d: %s\n", pin, get_mode_description(mode));
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestHardwareMain("test_hw_01", test_hw_01); }
