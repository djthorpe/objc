/**
 * @file main.c
 * @brief Pulse Width Modulation (PWM) Example
 *
 * This example demonstrates how to use the PWM function
 * to control the brightness of an LED on the Raspberry Pi Pico.
 *
 * In this example, we create a PWM signal on a GPIO pin to fade an LED in and
 * out. The PWM signal is generated using hardware PWM features of the RP2040.
 *
 * You can tune the PWM frequency and duty cycle to control the LED brightness.
 * In this example, the LED fades in and out smoothly.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

// GPIO pin used for PWM output (adjust as needed for your board)
#define PWM_PIN 25

typedef struct {
  int duty;
  bool rising;
  hw_pwm_t *pwm;
} pwm_state_t;

void pwm_wrap_callback(uint8_t slice, void *userdata) {
  pwm_state_t *state = (pwm_state_t *)userdata;
  if (!state || !state->pwm || !hw_pwm_valid(state->pwm) || state->pwm->unit != slice) {
    return; // Ignore if invalid or unexpected slice
  }

  if (state->rising) {
    if (++state->duty >= 100) {
      state->duty = 99;
      state->rising = false;
    }
  } else {
    if (--state->duty <= 0) {
      state->duty = 1;
      state->rising = true;
    }
  }

  // Update duty cycle (re-uses start API to adjust level)
  // hw_pwm_start updates the channel level and (re)enables the slice
  hw_pwm_start(state->pwm, PWM_PIN, (float)state->duty);
}

int main() {
  sys_init();
  hw_init();

  // Use 100Hz for fading the LED - then the fade frequency is ~1Hz
  hw_pwm_config_t config = hw_pwm_get_config(100.0f);

  sys_printf("Starting PWM on pin %d\n", PWM_PIN);

  // Determine PWM unit from GPIO and initialize
  uint8_t unit = hw_pwm_gpio_unit(PWM_PIN);
  hw_pwm_t pwm = hw_pwm_init(unit, &config);
  if (!hw_pwm_valid(&pwm)) {
    sys_printf("Failed to initialize PWM\n");
    return 1;
  }

  // Set the wrap callback function (updates duty each wrap)
  hw_pwm_set_callback(pwm_wrap_callback, &pwm);
  hw_pwm_set_irq_enabled(&pwm, true);

  // Start PWM at 0% duty (will begin ramping in callback)
  hw_pwm_start(&pwm, PWM_PIN, 0.0f);

  // Never stop
  while (1) {
    sys_sleep(1000);
  }

  sys_printf("Stopping PWM on pin %d\n", PWM_PIN);

  // Stop PWM
  hw_pwm_finalize(&pwm);

  hw_exit();
  sys_exit();
  return 0;
}
