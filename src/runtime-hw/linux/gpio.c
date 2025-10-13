#include <fcntl.h>
#include <linux/gpio.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// Define the GPIO banks and pins
#define GPIO_MAX_BANKS 4
#define GPIO_MAX_LINES HW_GPIO_MAX_COUNT

// Store file descriptors
static int chip_fds[GPIO_MAX_BANKS];
static int line_fds[GPIO_MAX_BANKS][GPIO_MAX_LINES];

// Synchronization
sys_mutex_t _gpio_mutex;
static sys_waitgroup_t _event_waitgroup;
static volatile bool _event_thread_stop = false;
static volatile int _epoll_fd = -1; // volatile for thread-safe checking

// GPIO callback
static hw_gpio_callback_t _gpio_callback = NULL;
static void *_gpio_callback_userdata = NULL;

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static int _gpio_open_chip(uint8_t bank);
static void _gpio_close_chip(uint8_t bank);
static void _gpio_release_line(uint8_t bank, uint8_t pin);
static void _gpio_remove_from_epoll(int line_fd);
static void _gpio_start_event_thread(void);
static void _gpio_stop_event_thread(void);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

// Mutex initialization
void _hw_gpio_init(void) {
#ifdef DEBUG
  sys_printf("_hw_gpio_init\n");
#endif
  _gpio_mutex = sys_mutex_init();

  // Initialize all chip and line fds to -1
  for (uint8_t bank = 0; bank < GPIO_MAX_BANKS; bank++) {
    chip_fds[bank] = -1;
    for (uint8_t pin = 0; pin < GPIO_MAX_LINES; pin++) {
      line_fds[bank][pin] = -1;
    }
  }

  // Start the event monitoring thread
  _gpio_start_event_thread();
}

// Mutex finalization
void _hw_gpio_finalize(void) {
#ifdef DEBUG
  sys_printf("_hw_gpio_finalize\n");
#endif

  // Stop event thread first
  _gpio_stop_event_thread();

  // Close all GPIO resources
  for (uint8_t bank = 0; bank < GPIO_MAX_BANKS; bank++) {
    _gpio_close_chip(bank);
  }

  sys_mutex_finalize(&_gpio_mutex);
}

/**
 * @brief Get the total number of available GPIO pins for a given bank.
 */
uint8_t hw_gpio_count(uint8_t bank) {
  int fd = _gpio_open_chip(bank);
  if (fd < 0) {
    return 0;
  }

  struct gpiochip_info info;
  if (ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, &info) < 0) {
    return 0;
  }

  if (info.lines > UINT8_MAX) {
    return 0; // Exceeds uint8_t range
  }
  return (uint8_t)info.lines;
}

/**
 * @brief Set the global GPIO interrupt callback handler.
 */
void hw_gpio_set_callback(hw_gpio_callback_t callback, void *userdata) {
  sys_mutex_lock(&_gpio_mutex);
  _gpio_callback = callback;
  _gpio_callback_userdata = userdata;
  sys_mutex_unlock(&_gpio_mutex);
}

/**
 * @brief Initialize a GPIO pin with the specified mode.
 */
hw_gpio_t hw_gpio_init(uint8_t bank, uint8_t pin, hw_gpio_mode_t mode) {
#ifdef DEBUG
  sys_printf("hw_gpio_init: initializing bank=%u pin=%u mode=%d\n", bank, pin,
             mode);
#endif
  hw_gpio_t gpio = {0};

  // Validate inputs
  if (bank >= GPIO_MAX_BANKS || pin >= GPIO_MAX_LINES) {
#ifdef DEBUG
    sys_printf("hw_gpio_init: invalid bank=%u or pin=%u\n", bank, pin);
#endif
    return gpio;
  }

  int chip_fd = _gpio_open_chip(bank);
  if (chip_fd < 0) {
#ifdef DEBUG
    sys_printf("hw_gpio_init: failed to open chip for bank=%u\n", bank);
#endif
    return gpio;
  }
  if (pin >= hw_gpio_count(bank)) {
#ifdef DEBUG
    sys_printf("hw_gpio_init: invalid pin=%u for bank=%u\n", pin, bank);
#endif
    return gpio;
  }

  // Validate mode
  if (mode != HW_GPIO_INPUT && mode != HW_GPIO_PULLUP &&
      mode != HW_GPIO_PULLDOWN && mode != HW_GPIO_OUTPUT) {
#ifdef DEBUG
    sys_printf("hw_gpio_init: invalid mode=%d\n", mode);
#endif
    return gpio;
  }

  // Release existing line if any
  _gpio_release_line(bank, pin);

  // Prepare GPIO v2 line request
  struct gpio_v2_line_request req = {0};
  req.offsets[0] = pin;
  req.num_lines = 1;
  sys_sprintf(req.consumer, sizeof(req.consumer), "hw_gpio");

  // Configure line based on mode
  uint64_t edge_flags =
      GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING;

  switch (mode) {
  case HW_GPIO_INPUT:
    req.config.flags = GPIO_V2_LINE_FLAG_INPUT | edge_flags;
    break;
  case HW_GPIO_PULLUP:
    req.config.flags =
        GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_UP | edge_flags;
    break;
  case HW_GPIO_PULLDOWN:
    req.config.flags =
        GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN | edge_flags;
    break;
  case HW_GPIO_OUTPUT:
    req.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    req.config.num_attrs = 1;
    req.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
    req.config.attrs[0].attr.values = 0; // Start with low
    req.config.attrs[0].mask = 1;
    break;
  default:
    // Unsupported mode
#ifdef DEBUG
    sys_printf("hw_gpio_init: unsupported mode=%d for bank=%u pin=%u\n", mode,
               bank, pin);
#endif
    return gpio;
  }

  // Request the line
  sys_mutex_lock(&_gpio_mutex);
  if (ioctl(chip_fd, GPIO_V2_GET_LINE_IOCTL, &req) < 0) {
    sys_mutex_unlock(&_gpio_mutex);
#ifdef DEBUG
    sys_printf("hw_gpio_init: failed to request line for bank=%u pin=%u\n",
               bank, pin);
#endif
    return gpio;
  }

  // Store the line file descriptor
  line_fds[bank][pin] = req.fd;

  // Add to epoll for input modes (with edge detection)
  if (mode != HW_GPIO_OUTPUT && _epoll_fd >= 0) {
    struct epoll_event ev = {0};
    ev.events = EPOLLIN;
    // Pack bank and pin into user data pointer (16-bit value)
    ev.data.ptr = (void *)(uintptr_t)((bank << 8) | pin);

    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, req.fd, &ev) < 0) {
#ifdef DEBUG
      sys_printf("hw_gpio_init: failed to add fd to epoll for bank=%u pin=%u\n",
                 bank, pin);
#endif
    }
  }

  sys_mutex_unlock(&_gpio_mutex);

#ifdef DEBUG
  sys_printf("hw_gpio_init: initialized bank=%u pin=%u mode=%d fd=%d\n", bank,
             pin, mode, req.fd);
#endif

  // Return success
  gpio.bank = bank;
  gpio.pin = pin;
  gpio.mask = (uint64_t)1 << pin; // Use 64-bit to handle all pin positions
  return gpio;
}

/**
 * @brief Finalize and release a GPIO pin.
 */
void hw_gpio_finalize(hw_gpio_t *gpio) {
  sys_assert(gpio);
  _gpio_release_line(gpio->bank, gpio->pin);
  // Clear struct to mark as invalid
  sys_memset(gpio, 0, sizeof(hw_gpio_t));
}

/**
 * @brief Get the current mode configuration of a GPIO pin.
 */
hw_gpio_mode_t hw_gpio_get_mode(hw_gpio_t *gpio) {
  sys_assert(gpio);
  if (gpio->bank >= GPIO_MAX_BANKS || gpio->pin >= GPIO_MAX_LINES) {
    return HW_GPIO_NONE;
  }

  sys_mutex_lock(&_gpio_mutex);
  int line_fd = line_fds[gpio->bank][gpio->pin];
  if (line_fd < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return HW_GPIO_NONE;
  }

  // Get line info from chip
  int chip_fd = chip_fds[gpio->bank];
  if (chip_fd < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return HW_GPIO_UNKNOWN;
  }

  struct gpio_v2_line_info info = {0};
  info.offset = gpio->pin;

  if (ioctl(chip_fd, GPIO_V2_GET_LINEINFO_IOCTL, &info) < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return HW_GPIO_UNKNOWN;
  }
  sys_mutex_unlock(&_gpio_mutex);

  // Determine mode from flags
  if (info.flags & GPIO_V2_LINE_FLAG_OUTPUT) {
    return HW_GPIO_OUTPUT;
  } else if (info.flags & GPIO_V2_LINE_FLAG_INPUT) {
    if (info.flags & GPIO_V2_LINE_FLAG_BIAS_PULL_UP) {
      return HW_GPIO_PULLUP;
    } else if (info.flags & GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN) {
      return HW_GPIO_PULLDOWN;
    } else {
      return HW_GPIO_INPUT;
    }
  }

  return HW_GPIO_UNKNOWN;
}

/**
 * @brief Set the current mode configuration of a GPIO pin.
 */
void hw_gpio_set_mode(hw_gpio_t *gpio, hw_gpio_mode_t mode) {
  sys_assert(gpio);

  // Re-initialize the GPIO with the new mode
  uint8_t bank = gpio->bank;
  uint8_t pin = gpio->pin;
  *gpio = hw_gpio_init(bank, pin, mode);
}

/**
 * @brief Read the current state of a GPIO pin.
 */
bool hw_gpio_get(hw_gpio_t *gpio) {
  sys_assert(gpio);
  if (gpio->bank >= GPIO_MAX_BANKS || gpio->pin >= GPIO_MAX_LINES) {
    return false;
  }

  sys_mutex_lock(&_gpio_mutex);
  int line_fd = line_fds[gpio->bank][gpio->pin];
  if (line_fd < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return false;
  }

  struct gpio_v2_line_values values = {0};
  values.mask = 1; // Read first line

  if (ioctl(line_fd, GPIO_V2_LINE_GET_VALUES_IOCTL, &values) < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return false;
  }
  sys_mutex_unlock(&_gpio_mutex);

  return (values.bits & 1) != 0;
}

/**
 * @brief Set the state of a GPIO pin.
 */
void hw_gpio_set(hw_gpio_t *gpio, bool value) {
  sys_assert(gpio);
  if (gpio->bank >= GPIO_MAX_BANKS || gpio->pin >= GPIO_MAX_LINES) {
    return;
  }

  sys_mutex_lock(&_gpio_mutex);
  int line_fd = line_fds[gpio->bank][gpio->pin];
  if (line_fd < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return;
  }

  struct gpio_v2_line_values values = {0};
  values.mask = 1; // Set first line
  values.bits = value ? 1 : 0;

  ioctl(line_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &values);
  sys_mutex_unlock(&_gpio_mutex);
}

///////////////////////////////////////////////////////////////////////////////
// EVENT THREAD

static void _gpio_event_thread(void *arg) {
  (void)arg;
#ifdef DEBUG
  sys_printf("_gpio_event_thread: event thread started\n");
#endif

  while (!_event_thread_stop) {
    // Wait for events with 100ms timeout
    struct epoll_event events[32]; // Process up to 32 events per iteration
    int nfds = epoll_wait(_epoll_fd, events, 32, 100);

    if (nfds < 0) {
      // Error in epoll_wait
#ifdef DEBUG
      sys_printf("_gpio_event_thread: epoll_wait error\n");
#endif
      sys_sleep(10);
      continue;
    }

    if (nfds == 0) {
      // Timeout, no events
      continue;
    }

    // Get callback (check if set before processing events)
    sys_mutex_lock(&_gpio_mutex);
    hw_gpio_callback_t callback = _gpio_callback;
    void *userdata = _gpio_callback_userdata;
    sys_mutex_unlock(&_gpio_mutex);

    if (!callback) {
      continue; // No callback set
    }

    // Process each event
    for (int i = 0; i < nfds; i++) {
      if (!(events[i].events & EPOLLIN)) {
        continue;
      }

      // Extract bank and pin from user data (stored as packed uint16_t)
      uint16_t data = (uint16_t)(uintptr_t)events[i].data.ptr;
      uint8_t event_bank = (data >> 8) & 0xFF;
      uint8_t event_pin = data & 0xFF;

      // Verify bank/pin are valid
      if (event_bank >= GPIO_MAX_BANKS || event_pin >= GPIO_MAX_LINES) {
        continue;
      }

      // Get the line FD
      sys_mutex_lock(&_gpio_mutex);
      int line_fd = line_fds[event_bank][event_pin];
      sys_mutex_unlock(&_gpio_mutex);

      if (line_fd < 0) {
        // FD was closed, skip it
        continue;
      }

      // Read the event (no mutex needed - reading from our own FD)
      struct gpio_v2_line_event event;
      ssize_t rd = read(line_fd, &event, sizeof(event));
      if (rd != sizeof(event)) {
        continue;
      }

      // Dispatch callback based on event type
      hw_gpio_event_t hw_event = 0;
      if (event.id == GPIO_V2_LINE_EVENT_RISING_EDGE) {
        hw_event = HW_GPIO_RISING;
      } else if (event.id == GPIO_V2_LINE_EVENT_FALLING_EDGE) {
        hw_event = HW_GPIO_FALLING;
      } else {
        continue; // Unknown event type
      }

#ifdef DEBUG
      sys_printf("_gpio_event_thread: event bank=%u pin=%u edge=%s\n",
                 event_bank, event_pin,
                 hw_event == HW_GPIO_RISING ? "RISING" : "FALLING");
#endif

      // Call the user callback
      callback(event_bank, event_pin, hw_event, userdata);
    }
  }

#ifdef DEBUG
  sys_printf("_gpio_event_thread: event thread stopping\n");
#endif
  // Signal completion
  sys_waitgroup_done(&_event_waitgroup);
}

static void _gpio_start_event_thread(void) {
#ifdef DEBUG
  sys_printf("_gpio_start_event_thread: starting event thread\n");
#endif

  // Initialize waitgroup first (even if thread creation fails)
  _event_waitgroup = sys_waitgroup_init();
  sys_waitgroup_add(&_event_waitgroup, 1);

  // Create epoll instance
  int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
  if (epoll_fd < 0) {
#ifdef DEBUG
    sys_printf("_gpio_start_event_thread: failed to create epoll instance\n");
#endif
    // Signal completion immediately since thread won't run
    sys_waitgroup_done(&_event_waitgroup);
    return;
  }

  _epoll_fd = epoll_fd;
  _event_thread_stop = false;

  // Start the event monitoring thread
  if (!sys_thread_create(_gpio_event_thread, NULL)) {
#ifdef DEBUG
    sys_printf("_gpio_start_event_thread: failed to create event thread\n");
#endif
    // Failed to create thread, clean up
    sys_waitgroup_done(&_event_waitgroup);
    close(epoll_fd);
    _epoll_fd = -1;
  }
}

static void _gpio_stop_event_thread(void) {
#ifdef DEBUG
  sys_printf("_gpio_stop_event_thread: stopping event thread\n");
#endif
  // Signal thread to stop
  _event_thread_stop = true;

  // Wait for thread to complete and cleanup
  sys_waitgroup_finalize(&_event_waitgroup);

  // Close epoll instance (thread has stopped, safe to close)
  // Save FD and mark as invalid atomically
  int epoll_fd = _epoll_fd;
  _epoll_fd = -1;

  if (epoll_fd >= 0) {
    close(epoll_fd);
  }
#ifdef DEBUG
  sys_printf("_gpio_stop_event_thread: event thread stopped\n");
#endif
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static int _gpio_open_chip(uint8_t bank) {
  if (bank >= GPIO_MAX_BANKS) {
    return -1;
  }

  sys_mutex_lock(&_gpio_mutex);
  if (chip_fds[bank] >= 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return chip_fds[bank]; // Already opened
  }

  char dev[32];
  sys_sprintf(dev, sizeof(dev), "/dev/gpiochip%u", bank);
  int fd = open(dev, O_RDONLY);
  if (fd < 0) {
#ifdef DEBUG
    sys_printf("_gpio_open_chip: failed to open %s\n", dev);
#endif
    sys_mutex_unlock(&_gpio_mutex);
    return -1;
  }

#ifdef DEBUG
  sys_printf("_gpio_open_chip: opened %s fd=%d\n", dev, fd);
#endif
  chip_fds[bank] = fd;
  sys_mutex_unlock(&_gpio_mutex);
  return fd;
}

static void _gpio_close_chip(uint8_t bank) {
  if (bank >= GPIO_MAX_BANKS) {
    return;
  }

  sys_mutex_lock(&_gpio_mutex);
  if (chip_fds[bank] < 0) {
    sys_mutex_unlock(&_gpio_mutex);
    return; // Not opened
  }

  // Release all line requests for this bank
  for (uint8_t pin = 0; pin < GPIO_MAX_LINES; pin++) {
    int line_fd = line_fds[bank][pin];
    if (line_fd >= 0) {
      line_fds[bank][pin] = -1; // Mark as released
      // Remove from epoll and close (safe to do with mutex held briefly)
      _gpio_remove_from_epoll(line_fd);
      close(line_fd);
    }
  }

  close(chip_fds[bank]);
  chip_fds[bank] = -1;
  sys_mutex_unlock(&_gpio_mutex);
}

static void _gpio_remove_from_epoll(int line_fd) {
  // Remove from epoll if it was added (safe to call even if not in epoll)
  int epoll_fd = _epoll_fd; // Read volatile once
  if (epoll_fd >= 0 && line_fd >= 0) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, line_fd, NULL);
  }
}

static void _gpio_release_line(uint8_t bank, uint8_t pin) {
  if (bank >= GPIO_MAX_BANKS || pin >= GPIO_MAX_LINES) {
    return;
  }

  sys_mutex_lock(&_gpio_mutex);
  int line_fd = line_fds[bank][pin];
  if (line_fd >= 0) {
    line_fds[bank][pin] = -1; // Mark as released first
    sys_mutex_unlock(&_gpio_mutex);

    // Remove from epoll and close outside mutex
    _gpio_remove_from_epoll(line_fd);
    close(line_fd);
  } else {
    sys_mutex_unlock(&_gpio_mutex);
  }
}
