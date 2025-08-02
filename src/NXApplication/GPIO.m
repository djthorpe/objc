#include <NXApplication/NXApplication.h>
#include <NXFoundation/NXFoundation.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

@implementation GPIO

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a pin with a mode.
 */
- (id)initPin:(uint8_t)pin mode:(hw_gpio_mode_t)mode {
  self = [super init];
  if (self == nil) {
    return nil; // Initialization failed
  }

  // Get the pin
  hw_gpio_t gpio = hw_gpio_init(pin, mode);
  if (gpio.mask == 0) {
    // Invalid pin initialization
    [self release];
    return nil;
  } else {
    _data = sys_malloc(sizeof(hw_gpio_t));
    if (_data == NULL) {
      // Memory allocation failed
      [self release];
      return nil;
    } else {
      // Store the pin data
      sys_memcpy(_data, &gpio, sizeof(hw_gpio_t));
    }
  }

  // Return success
  return self;
}

- (void)dealloc {
  if (_data) {
    sys_free(_data);
    _data = NULL;
  }
  [super dealloc];
}

/**
 * @brief Returns a GPIO input instance.
 */
+ (GPIO *)inputWithPin:(uint8_t)pin {
  return [[[self alloc] initPin:pin mode:HW_GPIO_INPUT] autorelease];
}

/**
 * @brief Returns a GPIO input instance, with pull-up resistor enabled.
 */
+ (GPIO *)pullupWithPin:(uint8_t)pin {
  return [[[self alloc] initPin:pin mode:HW_GPIO_PULLUP] autorelease];
}

/**
 * @brief Returns a GPIO input instance, with pull-up resistor enabled.
 */
+ (GPIO *)pulldownWithPin:(uint8_t)pin {
  return [[[self alloc] initPin:pin mode:HW_GPIO_PULLDOWN] autorelease];
}

/**
 * @brief Returns a GPIO output instance.
 */
+ (GPIO *)outputWithPin:(uint8_t)pin {
  return [[[self alloc] initPin:pin mode:HW_GPIO_OUTPUT] autorelease];
}

///////////////////////////////////////////////////////////////////////////////
// PROPERTIES

/**
 * @brief Returns the total number of GPIO pins available.
 * @return The total number of GPIO pins. Returns 0 if GPIO is not available.
 */
+ (uint8_t)count {
  return hw_gpio_count();
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

/**
 * @brief Returns the state of the GPIO pin.
 * @return true if the pin is high (logical 1), false if low (logical 0).
 */
- (BOOL)state {
  objc_assert(_data);
  hw_gpio_t *pin = (hw_gpio_t *)_data;
  objc_assert(pin->mask);
  return hw_gpio_get(pin);
}

/**
 * @brief Sets the state of the GPIO pin.
 * @param state true to set the pin high (logical 1), false to set low (logical
 * 0).
 */
- (void)setState:(BOOL)state {
  objc_assert(_data);
  hw_gpio_t *pin = (hw_gpio_t *)_data;
  objc_assert(pin->mask);
  hw_gpio_set(pin, state);
}

@end
