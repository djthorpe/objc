#pragma once
#include "infrared_rx.pio.h"
#include <hardware/clocks.h>
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <hardware/timer.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

typedef struct {
  uint8_t gpio;                       ///< GPIO pin number
  void *user_data;                    ///< Pointer to user data
  hw_infrared_rx_callback_t callback; ///< Pointer to callback function
  PIO pio;                            ///< PIO instance
  uint sm;                            ///< State machine number
  uint offset;                        ///< Program offset
  int8_t irq;                         ///< IRQ number
} hw_infrared_rx_ctx_t;

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static inline uint32_t _hw_infrared_rx_to_us(uint initial, uint32_t remaining);

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

// This is the max timeout count for IR signals
#define HW_INFRARED_RX_TIMEOUT_COUNT 0x7FFFFFFF

// This is the timeout duration for IR signals (50ms)
#define HW_INFRARED_RX_TIMEOUT_US 50000

// Use a sensible default clock divider for IR signals
// 10.0 gives ~800ns resolution and can measure up to ~3.4s
// This covers most IR protocols (NEC, RC5, Sony, etc.)
const float _hw_infrared_rx_clkdiv = 10.0f;

// Context for each PIO/SM combinations
hw_infrared_rx_ctx_t _hw_infrared_rx_ctx[NUM_PIOS * NUM_PIO_STATE_MACHINES] = {
    {0}};

///////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLER

static void _hw_infrared_rx_irq_pio_handler(void) {
  // Check only the PIO/SM combinations that are actually used for infrared
  for (unsigned int i = 0; i < NUM_PIOS * NUM_PIO_STATE_MACHINES; i++) {
    hw_infrared_rx_ctx_t *ctx = &_hw_infrared_rx_ctx[i];
    if (ctx->callback == NULL) {
      continue;
    }

    // Clear the FIFO, callback
    while (!pio_sm_is_rx_fifo_empty(ctx->pio, ctx->sm)) {
      uint32_t value = pio_sm_get(ctx->pio, ctx->sm);
      uint32_t count = value & HW_INFRARED_RX_TIMEOUT_COUNT;
      if (count == HW_INFRARED_RX_TIMEOUT_COUNT) {
        ctx->callback(HW_INFRARED_EVENT_TIMEOUT, 0, ctx->user_data);
        continue;
      }

      // Extract type from MSB and count from lower 31 bits
      // Treat long spaces as timeouts (typical IR timeout is > 50ms)
      bool is_space = (value & 0x80000000) != 0;
      uint32_t duration_us =
          _hw_infrared_rx_to_us(HW_INFRARED_RX_TIMEOUT_COUNT, count);

      if (is_space && duration_us > HW_INFRARED_RX_TIMEOUT_US) {
        ctx->callback(HW_INFRARED_EVENT_TIMEOUT, 0, ctx->user_data);
      } else if (is_space) {
        ctx->callback(HW_INFRARED_EVENT_SPACE, duration_us, ctx->user_data);
      } else {
        ctx->callback(HW_INFRARED_EVENT_MARK, duration_us, ctx->user_data);
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

static inline void *_hw_infrared_rx_pio_init(uint8_t gpio,
                                             hw_infrared_rx_callback_t callback,
                                             void *user_data) {
  sys_assert(gpio < hw_gpio_count());
  sys_assert(callback);

  // Claim a free state machine and add the program for the GPIO
  PIO pio;
  uint sm;
  uint offset;
  if (pio_claim_free_sm_and_add_program_for_gpio_range(
          &infrared_rx_program, &pio, &sm, &offset, gpio, 1, true) == false) {
    return NULL;
  }

  // If spot is already taken, return NULL, else claim the context
  hw_infrared_rx_ctx_t *ctx =
      &_hw_infrared_rx_ctx[(PIO_NUM(pio) * NUM_PIO_STATE_MACHINES) + sm];
  if (ctx->callback != NULL) {
    pio_remove_program_and_unclaim_sm(&infrared_rx_program, pio, sm, offset);
    return NULL;
  } else {
    // Initialize context atomically
    ctx->gpio = gpio;
    ctx->user_data = user_data;
    ctx->pio = pio;
    ctx->sm = sm;
    ctx->offset = offset;
    // Set callback last to mark context as active
    ctx->callback = callback;
  }

  // Configure the state machine
  pio_sm_config c = infrared_rx_program_get_default_config(ctx->offset);
  sm_config_set_clkdiv(&c, _hw_infrared_rx_clkdiv);
  sm_config_set_in_pins(&c, gpio);
  sm_config_set_jmp_pin(&c, gpio);
  sm_config_set_in_shift(&c, false, false, 32);
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

  // Find a free irq
  ctx->irq = pio_get_irq_num(ctx->pio, 0);
  if (irq_get_exclusive_handler(ctx->irq)) {
    ctx->irq++;
    if (irq_get_exclusive_handler(ctx->irq)) {
      // All IRQs are in use - cleanup and return error
      ctx->callback = NULL; // Mark context as free
      pio_remove_program_and_unclaim_sm(&infrared_rx_program, pio, sm, offset);
      return NULL;
    }
  }

  // Initialise the GPIO pin
  pio_gpio_init(ctx->pio, gpio);
  gpio_set_dir(gpio, GPIO_IN);
  gpio_pull_up(gpio);

  // Initialise the state machine
  pio_sm_init(ctx->pio, ctx->sm, ctx->offset, &c);

  // Enable interrupt - add a shared IRQ handler
  irq_add_shared_handler(ctx->irq, _hw_infrared_rx_irq_pio_handler,
                         PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
  irq_set_enabled(ctx->irq, true);

  // Set PIO to tell us when the FIFO is NOT empty
  const uint irq_index = ctx->irq - pio_get_irq_num(ctx->pio, 0);
  pio_set_irqn_source_enabled(
      ctx->pio, irq_index, pio_get_rx_fifo_not_empty_interrupt_source(ctx->sm),
      true);

  // Enable the state machine
  pio_sm_set_enabled(ctx->pio, ctx->sm, true);

  // Return success
  return ctx;
}

static inline void _hw_infrared_rx_pio_finalize(void *rx) {
  hw_infrared_rx_ctx_t *ctx = (hw_infrared_rx_ctx_t *)rx;
  sys_assert(ctx);
  sys_assert(ctx->callback);

  // Disable interrupt
  const uint irq_index = ctx->irq - pio_get_irq_num(ctx->pio, 0);
  pio_set_irqn_source_enabled(
      ctx->pio, irq_index, pio_get_rx_fifo_not_empty_interrupt_source(ctx->sm),
      false);
  irq_set_enabled(ctx->irq, false);
  irq_remove_handler(ctx->irq, _hw_infrared_rx_irq_pio_handler);

  // Remove the callback
  ctx->callback = NULL;

  // Free resources and unload our program
  pio_remove_program_and_unclaim_sm(&infrared_rx_program, ctx->pio, ctx->sm,
                                    ctx->offset);

  // Deinit the GPIO pin
  gpio_deinit(ctx->gpio);
}

static inline uint32_t _hw_infrared_rx_to_us(uint initial, uint32_t remaining) {
  if (remaining > initial) {
    // Indicate overflow or error
    return UINT32_MAX;
  }
  float pio_clk = (float)clock_get_hz(clk_sys) / _hw_infrared_rx_clkdiv;
  return (uint32_t)((float)(initial - remaining) * 1000000.0f / pio_clk);
}
