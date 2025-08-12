/**
 * @file examples/runtime/led/main.c
 * @brief LED control example
 *
 * This example demonstrates how to use the hardware LED
 * control functions to manage the state and brightness of an LED.
 *
 * It uses the on-board status LED on the Raspberry Pi Pico. If
 * the LED is not directly connected to a GPIO pin, but through
 * the Wi-Fi module, it will blink instead of fading.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// TIMER CALLBACK

void led_change(hw_led_t *led) {
  static int count = 0;
  hw_led_cap_t capabilities = hw_led_capabilities(led);
  switch (count++) {
  case 0:
    sys_printf("  Turning LED on\n");
    hw_led_set_state(led, true);
    break;
  case 1:
    sys_printf("  Turning LED off\n");
    hw_led_set_state(led, false);
    break;
  case 2:
    if (capabilities & HW_LED_CAP_LINEAR) {
      sys_printf("  Setting LED brightness to 20%%\n");
      hw_led_set_brightness(led, 51); // Set to 20% brightness
    }
    break;
  case 3:
    if (capabilities & HW_LED_CAP_LINEAR) {
      sys_printf("  Setting LED brightness to 50%%\n");
      hw_led_set_brightness(led, 128); // Set to 50% brightness
    }
    break;
  case 4:
    sys_printf("  Setting LED brightness to 100%%\n");
    hw_led_set_brightness(led, 255); // Set to 100% brightness
    break;
  case 5:
    sys_printf("  Setting LED brightness to 0%%\n");
    hw_led_set_brightness(led, 0); // Set to 0% brightness
    break;
  case 6:
    sys_printf("  Blink LED with interval 100ms\n");
    hw_led_blink(led, 100, true);
    break;
  case 7:
    sys_printf("  Fade LED with interval 250ms\n");
    hw_led_fade(led, 250, true);
    break;
  case 8:
    sys_printf("  Turning LED off\n");
    hw_led_set_state(led, false);
    break;
  case 9:
    sys_printf("  Blink LED once with 500ms interval\n");
    hw_led_blink(led, 500, false);
    break;
  case 10:
    sys_printf("  Fade LED once with 500ms interval\n");
    hw_led_fade(led, 500, false);
    break;
  default:
    sys_printf("  Turning LED off\n");
    hw_led_set_state(led, false);

    // Reset counter
    count = 0;
    break;
  }
}

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0
 */
bool core0_task() {
  // Use the on-board LED with associated PWM
  hw_pwm_t pwm;
  uint8_t gpio = hw_led_status_gpio();
  if (gpio != 0xFF) {
    pwm = hw_pwm_init(hw_pwm_gpio_unit(gpio), NULL); // Initialize PWM unit 0
  }
  hw_led_t led = hw_led_init(gpio, &pwm);
  if (!hw_led_valid(&led)) {
    sys_printf("core 0: Failed to initialize LED\n");
    return false;
  } else {
    sys_printf("LED GPIO: %d\n", hw_led_status_gpio());
  }

  hw_led_cap_t capabilities = hw_led_capabilities(&led);
  if (capabilities & HW_LED_CAP_BINARY) {
    sys_printf("  LED supports Binary control\n");
  }
  if (capabilities & HW_LED_CAP_LINEAR) {
    sys_printf("  LED supports Linear control via PWM\n");
  }
  if (capabilities & HW_LED_CAP_GPIO) {
    sys_printf("  LED supports GPIO control\n");
  }

  // Run the event processing loop - which is just sleeping for 1s
  do {
    led_change(&led);
    sys_sleep(5000);
  } while (true);

  // If we were to reach here, finalize the LED
  hw_led_finalize(&led);
  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();
  hw_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
