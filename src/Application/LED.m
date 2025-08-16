#include <Application/Application.h>
#include <runtime-hw/hw.h>

@implementation LED

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a pin with a mode.
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

- (void)dealloc {
  hw_led_finalize(&_led);
  [super dealloc];
}

/**
 * @brief Returns the on-board status LED.
 * @return An LED instance if the board exposes a status LED; nil otherwise.
 *
 * Returns an instance of the on-board status LED, which may be simple on/off or
 * able to display different brightness or colors.
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

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Set the LED on/off state.
 */
- (BOOL)setState:(BOOL)on {
  sys_assert(hw_led_valid(&_led));
  return hw_led_set_state(&_led, on) ? YES : NO;
}

@end
