#include <hardware/adc.h>
#include <hardware/gpio.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>

// Only include CYW43 headers if we actually have CYW43 GPIO pins
#if PICO_CYW43_SUPPORTED
#include <pico/cyw43_arch.h>
#include <pico/cyw43_driver.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static inline uint8_t _hw_power_vsys();
static inline uint8_t _hw_power_vbus();
static inline bool _hw_power_vsys_wifi();
static inline bool _hw_power_vbus_wifi();

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Handle for power management state.
 */
typedef struct hw_power_t {
  bool valid;
  uint8_t vsys;
  uint8_t vbus;
  uint64_t timestamp;
  uint8_t battery_percent;
  hw_power_flag_t source;
  hw_power_callback_t callback;
  void *user_data;
} hw_power_t;

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

#define HW_POWER_SAMPLE_COUNT 3
#define HW_POWER_MIN_VOLTAGE (3.0f)
#define HW_POWER_MAX_VOLTAGE (4.2f)
#define HW_POWER_UPDATE_INTERVAL_MS 2000

// The power singleton
static hw_power_t _hw_power = {0};

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize power management.
 */
hw_power_t *hw_power_init(uint8_t gpio_vsys, uint8_t gpio_vbus,
                          hw_power_callback_t callback, void *user_data) {
  // Reset the power handle
  hw_power_finalize(&_hw_power);

  // Set default for VSYS
  if (gpio_vsys == 0xFF) {
    gpio_vsys = _hw_power_vsys();
  }
  if (gpio_vbus == 0xFF && !_hw_power_vbus_wifi()) {
    gpio_vbus = _hw_power_vbus();
  }

  // Set parameters
  _hw_power.vbus = gpio_vbus;
  _hw_power.vsys = gpio_vsys;
  _hw_power.callback = callback;
  _hw_power.user_data = user_data;

  // Set valid
  _hw_power.valid = true;

  // Return initialized power handle
  return &_hw_power;
}

/**
 * @brief Determine if the power handle is initialized and usable.
 */
bool hw_power_valid(hw_power_t *power) {
  if (power == NULL) {
    return false;
  }
  return power->valid;
}

/**
 * @brief Finalize and release any resources.
 */
void hw_power_finalize(hw_power_t *power) {
  if (hw_power_valid(power) == false) {
    return;
  }

  // Reset power state
  sys_memset(power, 0, sizeof(hw_power_t));
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

// Does the WiFi chip use the VSYS pin?
static inline bool _hw_power_vsys_wifi() {
#ifdef CYW43_USES_VSYS_PIN
  return true;
#else
  return false;
#endif
}

// Is VBUS read through the WiFi?
static inline bool _hw_power_vbus_wifi() {
#ifdef CYW43_WL_GPIO_VBUS_PIN
  return true;
#else
  return false;
#endif
}

// Return the VSYS pin
static inline uint8_t _hw_power_vsys() {
#if defined(PICO_VSYS_PIN)
  return PICO_VSYS_PIN;
#elif defined(PICOLIPO_VBUS_DETECT_PIN)
  return PICOLIPO_VBUS_DETECT_PIN;
#else
  return 0xFF; // Invalid GPIO pin
#endif
}

// Return the VBUS pin - on GPIO
static inline uint8_t _hw_power_vbus() {
#ifdef CYW43_WL_GPIO_VBUS_PIN
  return CYW43_WL_GPIO_VBUS_PIN;
#elif defined(PICO_VBUS_PIN)
  return PICO_VBUS_PIN;
#elif defined(PICOLIPO_VBUS_DETECT_PIN)
  return PICOLIPO_VBUS_DETECT_PIN;
#else
  return 0xFF; // Invalid GPIO pin
#endif
}

/**
 * @brief Determine the current power source.
 */
static hw_power_flag_t _hw_power_source(uint8_t vbus) {
#if defined(PICO_CYW43_SUPPORTED) && defined(CYW43_WL_GPIO_VBUS_PIN)
  (void)vbus; // Not used when CYW43 handles VBUS
  return cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN) ? HW_POWER_USB
                                                     : HW_POWER_BATTERY;
#else /* No CYW43 VBUS path */
  if (vbus != 0xFF) {
    sys_assert(vbus < hw_gpio_count());
    gpio_set_function(vbus, GPIO_FUNC_SIO);
    return gpio_get(vbus) ? HW_POWER_USB : HW_POWER_BATTERY;
  }
  return HW_POWER_UNKNOWN;
#endif
}

/**
 * @brief Determine the current power voltage.
 */
static float _hw_power_voltage(uint8_t gpio, bool wifi) {
  if (gpio == 0xFF) {
    return 0;
  }
  if (wifi) {
#if defined(PICO_CYW43_SUPPORTED) && defined(CYW43_USES_VSYS_PIN) &&           \
    defined(CYW43_WL_GPIO_VBUS_PIN)
    cyw43_thread_enter();
    // Make sure cyw43 is awake
    cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);
#endif
  }

  // setup adc
  uint8_t chan = hw_adc_gpio_channel(gpio);
  if (chan == 0xFF) {
    return 0;
  }

  adc_gpio_init(gpio);
  adc_select_input(chan);
  adc_fifo_setup(true, false, 0, false, false);
  adc_run(true);

  // Ignore first values
  int ignore_count = HW_POWER_SAMPLE_COUNT;
  while (!adc_fifo_is_empty() && ignore_count-- > 0) {
    (void)adc_fifo_get_blocking();
  }

  // read vsys
  uint32_t vsys = 0;
  for (int i = 0; i < HW_POWER_SAMPLE_COUNT; i++) {
    uint16_t val = adc_fifo_get_blocking();
    vsys += val;
  }

  // Stop ADC
  adc_run(false);
  adc_fifo_drain();

  // Mean
  vsys /= HW_POWER_SAMPLE_COUNT;

  if (wifi) {
#if defined(PICO_CYW43_SUPPORTED) && defined(CYW43_USES_VSYS_PIN)
    cyw43_thread_exit();
#endif
  }

  // Return voltage
  const float conversion_factor = 3.3f / (1 << 12);
  return vsys * 3 * conversion_factor;
}

void _hw_power_poll() {
  hw_power_t *power = &_hw_power;
  if (hw_power_valid(power) == false) {
    return;
  }
  if (power->callback == NULL) {
    return;
  }

  // Determine if we should update
  uint64_t timestamp = sys_date_get_timestamp();
  if (power->timestamp != 0 &&
      timestamp < power->timestamp + HW_POWER_UPDATE_INTERVAL_MS) {
    return;
  } else {
    power->timestamp = timestamp;
  }

  // Perform the update
  bool updated = false;
  hw_power_flag_t source = hw_power_source(power);
  if (source != power->source) {
    power->source = source;
    updated = true;
  }
  uint8_t battery_percent = hw_power_battery_percent(power);
  if (battery_percent != power->battery_percent) {
    power->battery_percent = battery_percent;
    updated = true;
  }

  // Call the callback
  if (updated) {
    power->callback(power, power->source, power->battery_percent,
                    power->user_data);
  }
}

/**
 * @brief Notify power management system of reset.
 */
void _hw_power_notify_reset(uint32_t delay_ms) {
  hw_power_t *power = &_hw_power;
  if (hw_power_valid(power) == false) {
    return;
  }
  if (power->callback == NULL) {
    return;
  }
  power->callback(power, HW_POWER_RESET, delay_ms, power->user_data);
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Approximate battery state of charge, or zero if not available.
 */
uint8_t hw_power_battery_percent(hw_power_t *power) {
  sys_assert(power);

  // Get voltage
  float voltage = _hw_power_voltage(power->vsys, _hw_power_vsys_wifi());
  if (voltage == 0) {
    return 0;
  }

  // Scale voltage to percentage
  if (voltage < HW_POWER_MIN_VOLTAGE) {
    voltage = HW_POWER_MIN_VOLTAGE;
  }
  if (voltage > HW_POWER_MAX_VOLTAGE) {
    voltage = HW_POWER_MAX_VOLTAGE;
  }
  return (uint8_t)((voltage - HW_POWER_MIN_VOLTAGE) /
                   (HW_POWER_MAX_VOLTAGE - HW_POWER_MIN_VOLTAGE) * 100.0f);
}

/**
 * @brief Current detected power source(s).
 */
hw_power_flag_t hw_power_source(hw_power_t *power) {
  sys_assert(power);
  return _hw_power_source(power->vbus);
}
