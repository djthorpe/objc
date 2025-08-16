#include <Application/Application.h>
#include <runtime-hw/hw.h>

@implementation LED

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize the status LED
 */
- (id)init {
  self = [super init];
  if (self == nil) {
    return nil; // Initialization failed
  }

  // Initialize PWM
  uint8_t gpio = hw_led_status_gpio();
  hw_pwm_t pwm;
  if (gpio != 0xFF) {
    pwm = hw_pwm_init(hw_pwm_gpio_unit(gpio), NULL); // Initialize PWM
    if (!hw_pwm_valid(&pwm)) {
#ifdef DEBUG
      sys_printf("Failed to initialize PWM\n");
#endif
      [self release];
      return nil;
    }
  }

  // Initialize LED
  _led = hw_led_init(gpio, &pwm);
  if (!hw_led_valid(&_led)) {
#ifdef DEBUG
    sys_printf("Failed to initialize LED\n");
#endif
    [self release];
    return nil;
  }

  // Set state to "off"
  hw_led_set_state(&_led, false);

  // Return success
  return self;
}

/**
 * @brief Initialize an LED with PWM control
 */
- (id)initWithPin:(uint8_t)pin pwm:(BOOL)pwm {
  self = [super init];
  if (self == nil) {
    return nil; // Initialization failed
  }

  // Check pin
  if (pin >= hw_gpio_count()) {
    [self release];
    return nil; // Invalid pin number
  }

  // Get PWM adapter from GPIO pin
  if (pwm) {
    _pwm = hw_pwm_init(hw_pwm_gpio_unit(pin),
                       NULL); // Initialize PWM with default settings
    if (!hw_pwm_valid(&_pwm)) {
      [self release];
      return nil;
    }
  }

  // Initialize LED
  _led = hw_led_init(pin, &_pwm);
  if (!hw_led_valid(&_led)) {
#ifdef DEBUG
    sys_printf("Failed to initialize LED\n");
#endif
    [self release];
    return nil;
  }

  // Set state to "off"
  hw_led_set_state(&_led, false);

  // Return success
  return self;
}

- (void)dealloc {
  hw_led_finalize(&_led);
  hw_pwm_finalize(&_pwm);
  [super dealloc];
}

/**
 * @brief Returns the on-board status LED.
 * @return An LED instance if the board exposes a status LED; nil otherwise.
 *
 * Returns an instance of the on-board status LED, which may be simple on/off
 * or able to display different brightness or colors.
 *
 */
+ (LED *)status {
  static LED *status = nil;
  @synchronized(self) {
    if (status == nil) {
      status = [[LED alloc] init];
    }
    return status;
  }
}

/**
 * @brief Create or return a binary (on/off) LED on a GPIO pin.
 */
+ (LED *)binaryOnPin:(uint8_t)pin {
  return [[[LED alloc] initWithPin:pin pwm:NO] autorelease];
}

/**
 * @brief Create or return a linear-brightness LED on a GPIO/PWM-capable pin.
 */
+ (LED *)linearOnPin:(uint8_t)pin {
  return [[[LED alloc] initWithPin:pin pwm:YES] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Set the LED on/off state.
 */
- (BOOL)setState:(BOOL)on {
  sys_assert(hw_led_valid(&_led));
  return hw_led_set_state(&_led, on) ? YES : NO;
}

/**
 * @brief Set the LED with brightness
 */
- (BOOL)setBrightness:(uint8_t)brightness {
  sys_assert(hw_led_valid(&_led));
  return hw_led_set_brightness(&_led, brightness) ? YES : NO;
}

/**
 * @brief Blink with duration
 */
- (BOOL)blinkWithDuration:(NXTimeInterval)duration repeats:(BOOL)repeats {
  return NO;
}

/**
 * @brief Fade with duration
 */
- (BOOL)fadeWithDuration:(NXTimeInterval)duration repeats:(BOOL)repeats {
  return NO;
}

@end
