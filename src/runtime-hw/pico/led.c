// Platform / hardware
#include <hardware/gpio.h>
#ifdef PICO_CYW43_SUPPORTED
#include <pico/cyw43_arch.h>
#endif

// Runtime interfaces
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

// Std
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

struct hw_led_ctx {
  sys_timer_t timer;    ///< Timer for fade/blink operations
  bool repeats;         ///< True if blinking/fading repeats
  bool state;           ///< Current on/off state
  uint8_t toggle_count; ///< Reserved / future use
  uint8_t brightness;   ///< Current brightness (0-255)
  int8_t direction;     ///< Fade direction: +1 up, -1 down
  bool internal_update; ///< True while internally adjusting output
  bool valid;           ///< Context valid flag
};

static inline struct hw_led_ctx *hw_led_get_ctx(hw_led_t *led) {
  return (struct hw_led_ctx *)led->ctx;
}

// Forward declaration of blink callback
static void _hw_led_blink_callback(sys_timer_t *timer);
static void _hw_led_fade_callback(sys_timer_t *timer);
static bool _hw_led_output(hw_led_t *led, bool on);

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
  struct hw_led_ctx *ctx = hw_led_get_ctx(&led);
  sys_memset(ctx, 0, sizeof(struct hw_led_ctx));
  ctx->valid = true;
  ctx->brightness = 0;
  ctx->direction = 1;
  ctx->state = false;
  (void)_hw_led_output(&led, false); // Ensure off
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
  if (!led) {
    return false;
  }
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (!ctx || !ctx->valid) {
    return false; // Must have been initialized and not finalized
  }
  // Wi-Fi status LED (virtual GPIO) – treat as valid once ctx->valid is set
  if (led->gpio == 0xFF && hw_led_status_is_wifi()) {
    return true;
  }
  if (led->gpio >= hw_gpio_count()) {
    return false;
  }
  if (led->pwm) {
    return hw_pwm_valid(led->pwm);
  }
  return true;
}

/**
 * @brief Finalize and release an LED.
 */
void hw_led_finalize(hw_led_t *led) {
  if (led == NULL) {
    return;
  }
  if (!hw_led_valid(led)) {
    return;
  }
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (ctx && sys_timer_valid(&ctx->timer)) {
    sys_timer_finalize(&ctx->timer);
  }
  (void)_hw_led_output(led, false);
  if (led->pwm && hw_pwm_valid(led->pwm)) {
    hw_pwm_stop(led->pwm);
  }
  if (led->gpio != 0xFF && led->gpio < hw_gpio_count()) {
    gpio_deinit(led->gpio);
  }
  if (ctx) {
    ctx->valid = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Set the WiFi-connected LED state.
 */
static inline bool _hw_led_set_state_wifi(bool on) {
#ifdef PICO_CYW43_SUPPORTED
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
  return true;
#else
  (void)on;     // Suppress unused parameter warning
  return false; // Not supported if Wi-Fi is not available
#endif
}

/**
 * @brief Internal output helper which bypasses cancelling timers
 */
static bool _hw_led_output(hw_led_t *led, bool on) {
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (led->pwm == NULL) {
    if (led->gpio == 0xFF && hw_led_status_is_wifi()) {
      bool r = _hw_led_set_state_wifi(on);
      if (r && ctx) {
        ctx->brightness = on ? 0xFF : 0x00;
      }
      return r;
    }
    sys_assert(led->gpio < hw_gpio_count());
    gpio_put(led->gpio, on ? 1 : 0);
    if (ctx) {
      ctx->brightness = on ? 0xFF : 0x00;
    }
    return true;
  } else {
    bool r = hw_pwm_start(led->pwm, led->gpio, on ? 100.0f : 0.0f);
    if (r && ctx) {
      ctx->brightness = on ? 0xFF : 0x00;
    }
    return r;
  }
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Return LED capabilities.
 */
hw_led_cap_t hw_led_capabilities(hw_led_t *led) {
  if (!hw_led_valid(led)) {
    return HW_LED_CAP_NONE;
  }
  hw_led_cap_t caps = HW_LED_CAP_BINARY;
  if (led->pwm && hw_pwm_valid(led->pwm)) {
    caps |= HW_LED_CAP_LINEAR;
  }
  if (led->gpio != 0xFF) {
    caps |= HW_LED_CAP_GPIO;
  }
  return caps;
}

/**
 * @brief Set the LED state.
 */
bool hw_led_set_state(hw_led_t *led, bool on) {
  if (!hw_led_valid(led)) {
    return false;
  }
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (!ctx || !ctx->valid) {
    return false; // Must have been initialized and not finalized
  }

  // Cancel any ongoing fade or blink operations unless internal update
  bool internal = ctx ? ctx->internal_update : false;
  if (!internal) {
    if (led->pwm && hw_pwm_valid(led->pwm)) {
      hw_pwm_set_irq_enabled(led->pwm, false);
    }
    if (ctx && sys_timer_valid(&ctx->timer)) {
      sys_timer_finalize(&ctx->timer);
    }
  }

  return _hw_led_output(led, on);
}

/**
 * @brief Set the LED brightness.
 */
bool hw_led_set_brightness(hw_led_t *led, uint8_t brightness) {
  if (!hw_led_valid(led)) {
    return false;
  }
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (!ctx || !ctx->valid) {
    return false; // Must have been initialized and not finalized
  }

  // Cancel any ongoing fade or blink operations unless internal update
  bool internal = ctx ? ctx->internal_update : false;
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
    bool r =
        hw_pwm_start(led->pwm, led->gpio, (float)brightness * 100.0f / 255.0f);
    if (r && ctx) {
      ctx->brightness = brightness;
    }
    return r;
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
  // Treat period_ms as full ON+OFF cycle; compute half-period toggle
  // interval Enforce minimum half-period of 100ms (=> 200ms full cycle) for
  // stability
  if (period_ms < 200) {
    period_ms = 200; // Clamp
  }
  uint32_t half_period = period_ms / 2;
  if (half_period == 0) {
    half_period = 100; // Fallback
  }

  // Cancel any existing PWM fade/blink IRQ and timer
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (!ctx || !ctx->valid) {
    return false; // Must have been initialized and not finalized
  }
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

  // Create and start timer (toggle each half-period)
  ctx->timer = sys_timer_init(half_period, led, _hw_led_blink_callback);
  if (ctx->timer.callback == NULL) { // Invalid initialization
    return false;
  }
  if (!sys_timer_start(&ctx->timer)) {
    return false;
  }
  return true;
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
  if (!ctx || !ctx->valid) {
    return false; // Must have been initialized and not finalized
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
  if (ctx->timer.callback == NULL) {
    return false;
  }
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
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (!hw_led_valid(led) || !ctx || !ctx->valid) {
    // Stop further callbacks if LED became invalid mid-operation
    if (ctx) {
      sys_timer_finalize(&ctx->timer);
    }
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
  struct hw_led_ctx *ctx = hw_led_get_ctx(led);
  if (!hw_led_valid(led) || !ctx || !ctx->valid) {
    if (ctx) {
      sys_timer_finalize(&ctx->timer);
    }
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
