#include <hardware/gpio.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#ifdef PICO_CYW43_SUPPORTED
#include <pico/cyw43_arch.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// TYPES

struct hw_led_ctx {
  sys_timer_t timer;    ///< Timer for fade/blink operations
  bool repeats;         ///< True if blinking repeats
  bool state;           ///< Current on/off state during blink
  uint8_t toggle_count; ///< Number of toggles performed (for one-shot)
  uint8_t brightness;   ///< Current brightness for fade (0-255)
  int8_t direction;     ///< Fade direction: +1 up, -1 down
  bool internal_update; ///< True while internally adjusting output
};

static inline struct hw_led_ctx *hw_led_get_ctx(hw_led_t *led) {
  return (struct hw_led_ctx *)led->ctx;
}

// Forward declaration of blink callback
static void _hw_led_blink_callback(sys_timer_t *timer);
static void _hw_led_fade_callback(sys_timer_t *timer);

// Internal output helper which bypasses cancelling timers
static bool _hw_led_output(hw_led_t *led, bool on) {
  if (led->pwm == NULL) {
    if (led->gpio == 0xFF && hw_led_status_is_wifi()) {
      return hw_led_set_state_wifi(on);
    }
    sys_assert(led->gpio < hw_gpio_count());
    gpio_put(led->gpio, on ? 1 : 0);
    return true;
  } else {
    return hw_pwm_start(led->pwm, led->gpio, on ? 100.0f : 0.0f);
  }
}

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static inline bool hw_led_status_is_wifi() {
#ifdef CYW43_WL_GPIO_LED_PIN
  return true;
#else
  return false;
#endif
}

static inline bool hw_led_status_is_ws2812() {
#ifdef PICO_DEFAULT_WS2812_PIN
  return true;
#else
  return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a LED unit.
 */
hw_led_t hw_led_init(uint8_t gpio, hw_pwm_t *pwm) {
  sys_assert(gpio < hw_gpio_count() || gpio == 0xFF);
  sys_assert(pwm == NULL || hw_pwm_valid(pwm));
  sys_assert(HW_LED_CTX_SIZE >= sizeof(struct hw_led_ctx));

  // If it's WS2812 we don't support it yet
  hw_led_t led = {0};
  if (gpio == 0xFF && hw_led_status_is_ws2812()) {
    sys_printf("WS2812 LED control not implemented yet\n");
    return led;
  }

  // If gpio is 0xFF, assume it's the on-board status LED
  if (gpio == 0xFF) {
    gpio = hw_led_status_gpio();
  }

  // Check GPIO pin validity against the PWM
  if (gpio != 0xFF && pwm) {
    uint8_t pwm_unit = hw_pwm_gpio_unit(gpio);
    if (pwm->unit != pwm_unit) {
      return led; // Return invalid LED
    }
  }

  // Set the GPIO to output mode if not PWM
  if (gpio != 0xFF && pwm == NULL) {
    sys_assert(gpio < hw_gpio_count());
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_OUT);
  }

  // Initialize the struct
  led.gpio = gpio;
  led.pwm = pwm;

  // Set state to off - we initialize the PWM in here
  hw_led_set_state(&led, false);

  // Return valid LED structure
  return led;
}

/**
 * @brief Return the GPIO pin number for the on-board status LED.
 */
uint8_t hw_led_status_gpio() {
#ifdef PICO_DEFAULT_LED_PIN
  return PICO_DEFAULT_LED_PIN; // Return the default LED pin
#elif defined(PICO_DEFAULT_WS2812_PIN)
  return PICO_DEFAULT_WS2812_PIN; // Return the default WS2812 pin
#else
  return 0xFF; // Return 0xFF if the GPIO is controlled through
               // the Wi-Fi subsystem, or not available
#endif
}

/**
 * @brief Check if an LED is valid.
 */
bool hw_led_valid(hw_led_t *led) {
  if (led == NULL) {
    return false;
  }
  if (led->gpio == 0xFF) {
    // If it's WiFi, then OK
    return hw_led_status_is_wifi();
  }
  if (led->gpio >= hw_gpio_count()) {
    return false; // Invalid GPIO pin
  }
  if (led->pwm && !hw_pwm_valid(led->pwm)) {
    return false; // Invalid PWM structure
  }
  return true; // Valid LED structure
}

/**
 * @brief Finalize and release an LED.
 */
void hw_led_finalize(hw_led_t *led) {
  if (led == NULL || !hw_led_valid(led)) {
    return; // Nothing to finalize
  }

  // Finalize any timers
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (ctx && sys_timer_valid(&ctx->timer)) {
    sys_timer_finalize(&ctx->timer);
  }

  // Set state to off
  hw_led_set_state(led, false);

  // If PWM is used, stop it
  if (led->pwm) {
    hw_pwm_stop(led->pwm);
  }

  // Deinitialize the GPIO pin
  if (led->gpio != 0xFF) {
    sys_assert(led->gpio < hw_gpio_count());
    gpio_deinit(led->gpio); // Deinitialize the GPIO pin
  }

  // Clear the LED structure
  sys_memset(led, 0, sizeof(hw_led_t));
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Set the LED state.
 */
bool hw_led_set_state_wifi(bool on) {
#ifdef PICO_CYW43_SUPPORTED
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
  return true;
#else
  (void)on;     // Suppress unused parameter warning
  return false; // Not supported if Wi-Fi is not available
#endif
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Return LED capabilities.
 */
hw_led_cap_t hw_led_capabilities(hw_led_t *led) {
  sys_assert(led && hw_led_valid(led));
  hw_led_cap_t capabilities = HW_LED_CAP_BINARY; // Default to binary control

  if (led->pwm && hw_pwm_valid(led->pwm)) {
    capabilities |= HW_LED_CAP_LINEAR; // Supports PWM control
  }
  if (led->gpio != 0xFF && !hw_led_status_is_wifi()) {
    capabilities |= HW_LED_CAP_GPIO; // Supports GPIO control
  }
  return capabilities;
}

/**
 * @brief Set the LED state.
 */
bool hw_led_set_state(hw_led_t *led, bool on) {
  sys_assert(led && hw_led_valid(led));
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  bool internal = ctx ? ctx->internal_update : false;

  // Cancel any ongoing fade or blink operations unless internal update
  if (!internal) {
    if (led->pwm && hw_pwm_valid(led->pwm)) {
      hw_pwm_set_irq_enabled(led->pwm, false);
    }
    if (ctx && sys_timer_valid(&ctx->timer)) {
      sys_timer_finalize(&ctx->timer);
    }
  }

  // Binary control
  if (led->pwm == NULL) {
    if (led->gpio == 0xFF && hw_led_status_is_wifi()) {
      return hw_led_set_state_wifi(on);
    }
    sys_assert(led->gpio < hw_gpio_count());
    gpio_put(led->gpio, on ? 1 : 0);
    return true;
  }

  // Linear control using PWM
  if (led->pwm) {
    return hw_pwm_start(led->pwm, led->gpio,
                        on ? 100.0f : 0.0f); // Set to full brightness or off
  }

  // Don't know what to do here
  return false;
}

/**
 * @brief Set the LED brightness.
 */
bool hw_led_set_brightness(hw_led_t *led, uint8_t brightness) {
  sys_assert(led && hw_led_valid(led));
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  bool internal = ctx ? ctx->internal_update : false;

  // Cancel any ongoing fade or blink operations unless internal update
  if (!internal) {
    if (led->pwm && hw_pwm_valid(led->pwm)) {
      hw_pwm_set_irq_enabled(led->pwm, false);
    }
    if (ctx && sys_timer_valid(&ctx->timer)) {
      sys_timer_finalize(&ctx->timer);
    }
  }

  // Binary control
  if (led->pwm == NULL) {
    if (brightness == 0x00) {
      hw_led_set_state(led, false); // Set to off
      return true;
    } else if (brightness == 0xFF) {
      hw_led_set_state(led, true); // Set to full brightness
      return true;
    } else {
      return false; // Can't set intermediate brightness without PWM
    }
  }

  // Linear control using PWM
  if (led->pwm) {
    hw_pwm_start(led->pwm, led->gpio, (float)brightness * 100.0f / 255.0f);
    return true;
  }

  // Don't know what to do here
  return false;
}

/**
 * @brief Blink the LED continuously.
 */
bool hw_led_blink(hw_led_t *led, uint32_t period_ms, bool repeats) {
  sys_assert(led && hw_led_valid(led));

  // Check invalid period
  if (period_ms == 0) {
    return false;
  }
  // Enforce minimum period of 250ms
  if (period_ms < 250) {
    period_ms = 250;
  }

  // Cancel any existing PWM fade/blink IRQ and timer
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  sys_assert(ctx);
  if (led->pwm && hw_pwm_valid(led->pwm)) {
    hw_pwm_set_irq_enabled(led->pwm, false);
  }
  if (sys_timer_valid(&ctx->timer)) {
    sys_timer_finalize(&ctx->timer);
  }

  // Initialize blink state (start ON immediately)
  ctx->repeats = repeats;
  ctx->state = true;     // Start ON
  ctx->toggle_count = 0; // Not used for one-shot now
  ctx->internal_update = true;
  _hw_led_output(led, true);
  ctx->internal_update = false;

  // Create and start timer (toggle each period_ms)
  ctx->timer = sys_timer_init(period_ms, led, _hw_led_blink_callback);
  if (!sys_timer_valid(&ctx->timer)) {
    return false;
  } else {
    return sys_timer_start(&ctx->timer);
  }
}

/**
 * @brief Fade the LED continuously.
 */
bool hw_led_fade(hw_led_t *led, uint32_t period_ms, bool repeats) {
  sys_assert(led && hw_led_valid(led));

  // Check invalid period
  if (period_ms == 0) {
    return false;
  }

  // If no PWM (no linear capability) fallback to blink per documentation
  if (led->pwm == NULL || !hw_pwm_valid(led->pwm)) {
    // Use same period for blink (approximate behavior)
    return hw_led_blink(led, period_ms, repeats);
  }

  // Cancel any existing PWM IRQ and timer
  hw_pwm_set_irq_enabled(led->pwm, false);
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (ctx == NULL) {
    return false;
  }
  if (sys_timer_valid(&ctx->timer)) {
    sys_timer_finalize(&ctx->timer);
  }

  // Initialize fade parameters
  ctx->repeats = repeats;
  ctx->brightness = 0;   // Start at 0
  ctx->direction = 1;    // Fade up initially
  ctx->state = true;     // Consider LED logically active
  ctx->toggle_count = 0; // Unused for fade

  // Set initial brightness
  hw_pwm_start(led->pwm, led->gpio, 0.0f);

  // Determine timer interval for smooth fade
  // We define 'period_ms' as the time for a full up+down cycle.
  // Up cycle has 255 steps; down cycle has 255 steps => 510 steps total.
  uint32_t steps_per_cycle = 510;
  uint32_t interval = (uint32_t)period_ms / steps_per_cycle;
  if (interval == 0) {
    interval = 1; // Minimum 1ms interval
  }

  ctx->timer = sys_timer_init(interval, led, _hw_led_fade_callback);
  if (!sys_timer_start(&ctx->timer)) {
    return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS

static void _hw_led_blink_callback(sys_timer_t *timer) {
  if (timer == NULL || timer->userdata == NULL) {
    return;
  }
  hw_led_t *led = (hw_led_t *)timer->userdata;
  if (!hw_led_valid(led)) {
    return;
  }
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (ctx == NULL) {
    return;
  }

  // Toggle state (internal update to avoid cancelling timer)
  ctx->state = !ctx->state;
  ctx->internal_update = true;
  _hw_led_output(led, ctx->state);
  ctx->internal_update = false;

  if (!ctx->repeats && ctx->state == false) {
    // One-shot semantics: initial ON then OFF -> stop
    sys_timer_finalize(&ctx->timer);
    ctx->toggle_count = 0;
  }
}

static void _hw_led_fade_callback(sys_timer_t *timer) {
  if (timer == NULL || timer->userdata == NULL) {
    return;
  }
  hw_led_t *led = (hw_led_t *)timer->userdata;
  if (!hw_led_valid(led)) {
    return;
  }
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (ctx == NULL) {
    return;
  }
  if (led->pwm == NULL || !hw_pwm_valid(led->pwm)) {
    // Should not occur: finalize timer
    sys_timer_finalize(&ctx->timer);
    return;
  }

  // Advance brightness
  int next = (int)ctx->brightness + (int)ctx->direction;
  if (next >= 255) {
    next = 255;
    ctx->direction = -1; // Reverse
  } else if (next <= 0) {
    next = 0;
    if (ctx->direction == -1) {
      // Completed a full up+down cycle
      if (!ctx->repeats) {
        hw_pwm_start(led->pwm, led->gpio, 0.0f); // Ensure off
        sys_timer_finalize(&ctx->timer);
        return;
      }
      ctx->direction = 1; // Start rising again
    }
  }
  ctx->brightness = (uint8_t)next;

  float duty = (float)ctx->brightness * 100.0f / 255.0f;
  ctx->internal_update = true;
  hw_pwm_start(led->pwm, led->gpio, duty);
  ctx->internal_update = false;
}
