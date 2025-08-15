#include "GPIO+Private.h"
#include <NXApplication/NXApplication.h>
#include <NXFoundation/NXFoundation.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

// Delegate
static id<GPIODelegate> _gpioDelegate = nil;

// GPIO instances
static GPIO *_gpio[HW_GPIO_MAX_COUNT] = {0};

///////////////////////////////////////////////////////////////////////////////
// CALLBACKS

void _gpio_callback(uint8_t pin, hw_gpio_event_t event) {
  if (event == 0) {
    return; // No event to process
  }
  if (pin >= HW_GPIO_MAX_COUNT) {
    return; // Invalid pin number
  }

  // Get the GPIO instance for the pin
  GPIO *gpio = _gpio[pin];
  if (gpio) {
    [gpio changed:(GPIOEvent)event];
  }
}

///////////////////////////////////////////////////////////////////////////////

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

  // Check maximum number of pins
  if (pin >= hw_gpio_count()) {
    [self release];
    return nil;
  }

  // Get the pin
  _pin = hw_gpio_init(pin, mode);
  if (hw_gpio_valid(&_pin) == false) {
    // Invalid pin initialization
    [self release];
    return nil;
  } else {
    _mode = hw_gpio_get_mode(&_pin);
  }

  // Return success
  return self;
}

- (void)dealloc {
  sys_assert(self == _gpio[_pin.pin]);
  hw_gpio_finalize(&_pin);
  if (self == _gpio[_pin.pin]) {
    sys_assert(self == _gpio[_pin.pin]);
    _gpio[_pin.pin] = nil; // Clear the static GPIO instance
  }
  hw_gpio_finalize(&_pin);
  [super dealloc];
}

/**
 * @brief Returns a GPIO input instance.
 */
+ (GPIO *)inputWithPin:(uint8_t)pin {
  // Check input arguments
  if (pin >= hw_gpio_count() || pin >= HW_GPIO_MAX_COUNT) {
    return nil; // Invalid pin number
  }

  // Get or create static instance of GPIO pin
  @synchronized(self) {
    GPIO *gpio = _gpio[pin];
    if (gpio == nil) {
      _gpio[pin] = [[self alloc] initPin:pin mode:HW_GPIO_INPUT];
    }

    // Return retained instance
    return _gpio[pin];
  }
}

/**
 * @brief Returns a GPIO input instance, with pull-up resistor enabled.
 */
+ (GPIO *)pullupWithPin:(uint8_t)pin {
  // Check input arguments
  if (pin >= hw_gpio_count() || pin >= HW_GPIO_MAX_COUNT) {
    return nil; // Invalid pin number
  }

  // Get or create static instance of GPIO pin
  @synchronized(self) {
    GPIO *gpio = _gpio[pin];
    if (gpio == nil) {
      _gpio[pin] = [[self alloc] initPin:pin mode:HW_GPIO_PULLUP];
    }

    // Return retained instance
    return _gpio[pin];
  }
}

/**
 * @brief Returns a GPIO input instance, with pull-down resistor enabled.
 */
+ (GPIO *)pulldownWithPin:(uint8_t)pin {
  // Check input arguments
  if (pin >= hw_gpio_count() || pin >= HW_GPIO_MAX_COUNT) {
    return nil; // Invalid pin number
  }

  // Get or create static instance of GPIO pin
  @synchronized(self) {
    GPIO *gpio = _gpio[pin];
    if (gpio == nil) {
      _gpio[pin] = [[self alloc] initPin:pin mode:HW_GPIO_PULLDOWN];
    }

    // Return retained instance
    return _gpio[pin];
  }
}

/**
 * @brief Returns a GPIO output instance.
 */
+ (GPIO *)outputWithPin:(uint8_t)pin {
  // Check input arguments
  if (pin >= hw_gpio_count() || pin >= HW_GPIO_MAX_COUNT) {
    return nil; // Invalid pin number
  }

  // Get or create static instance of GPIO pin
  @synchronized(self) {
    GPIO *gpio = _gpio[pin];
    if (gpio == nil) {
      _gpio[pin] = [[self alloc] initPin:pin mode:HW_GPIO_OUTPUT];
    }

    // Return retained instance
    return _gpio[pin];
  }
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

/**
 * @brief Returns the pin number.
 */
- (uint8_t)pin {
  return _pin.pin;
}

/**
 * @brief Returns true if the GPIO pin is configured as an input.
 */
- (bool)isInput {
  return _mode == HW_GPIO_INPUT || _mode == HW_GPIO_PULLUP ||
         _mode == HW_GPIO_PULLDOWN;
}

/**
 * @brief Returns true if the GPIO pin is configured as an output.
 */
- (bool)isOutput {
  return _mode == HW_GPIO_OUTPUT;
}

///////////////////////////////////////////////////////////////////////////////
// METHODS

+ (void)setDelegate:(id<GPIODelegate>)delegate {
  @synchronized(self) {
    // Set the GPIO delegate
    _gpioDelegate = delegate;
  }
}

+ (id<GPIODelegate>)delegate {
  @synchronized(self) {
    return _gpioDelegate;
  }
}

/**
 * @brief Returns the state of the GPIO pin.
 * @return true if the pin is high (logical 1), false if low (logical 0).
 */
- (BOOL)state {
  objc_assert(hw_gpio_valid(&_pin));
  return hw_gpio_get(&_pin);
}

/**
 * @brief Sets the state of the GPIO pin.
 * @param state true to set the pin high (logical 1), false to set low (logical
 * 0).
 */
- (void)setState:(BOOL)state {
  objc_assert(hw_gpio_valid(&_pin));
  hw_gpio_set(&_pin, state);
}

/**
 * @brief Calls the delegate's gpio:changed: method. If GPIO is subclassed,
 * the subclass's implementation will be called instead.
 */
- (void)changed:(GPIOEvent)event {
  // Call the delegate GPIO event handler
  if (_gpioDelegate &&
      object_respondsToSelector(_gpioDelegate, @selector(gpio:changed:))) {
    [_gpioDelegate gpio:self changed:(GPIOEvent)event];
  }
}

///////////////////////////////////////////////////////////////////////////////
// OBJECT PROTOCOLS

- (NXString *)description {
  return [NXString stringWithFormat:@"(gpio pin=%d)", (int)_pin.pin];
}

@end
