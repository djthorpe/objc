/**
 * @file main.c
 * @brief Simple PWM Blink Test
 */

#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

// Pin - use LED pink definitions
#define PWM_PIN 25

void pwm_wrap_callback(uint8_t slice, void *userdata) {
  (void)slice; // Suppress unused parameter warning

  static int duty = 0;
  static bool going_up = true;
  if (going_up) {
    ++duty;
    if (duty > 100) {
      duty = 100;
      going_up = false;
    }
  } else {
    --duty;
    if (duty < 0) {
      duty = 0;
      going_up = true;
    }
  }

  // Set duty cycle for both channels
  hw_pwm_set_duty(userdata, 0, duty);
  hw_pwm_set_duty(userdata, 1, duty);
}

int main() {
  sys_init();
  hw_init();
  sys_printf("PWM Blink Test\n");

  // User 100Hz
  hw_pwm_config_t config = hw_pwm_get_config(100);

  sys_printf("Starting PWM on pin %d\n", PWM_PIN);

  // Initialize PWM
  hw_pwm_t pwm = hw_pwm_init(PWM_PIN, 0, &config);
  if (!hw_pwm_valid(&pwm)) {
    sys_printf("Failed to initialize PWM\n");
    return 1;
  }

  // Set the wrap callback function
  hw_pwm_set_callback(pwm_wrap_callback,
                      &pwm); // No callback for this simple test
  hw_pwm_set_irq_enabled(&pwm, true);

  // Start PWM
  hw_pwm_start(&pwm);

  sys_printf("Starting PWM on pin %d\n", PWM_PIN);

  // Never stop
  while (1) {
    sys_sleep(1000);
  }

  sys_printf("Stopping PWM on pin %d\n", PWM_PIN);

  // Stop PWM
  hw_pwm_finalize(&pwm);

  sys_printf("Ended\n");

  hw_exit();
  sys_exit();
  return 0;
}
