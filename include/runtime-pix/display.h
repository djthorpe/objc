/**
 * @file display.h
 * @brief Display functions.
 * @defgroup PixelDisplay Pixel Displays
 * @ingroup Pixel
 *
 * Functions for opening and closing pixel displays.
 */
#pragma once
#include "frame.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

struct pix_display_t;

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

/**
 * @brief Size of the display context buffer.
 * @ingroup PixelDisplay
 *
 * This defines the size in bytes of the internal context buffer used for
 * display operations. The buffer must be large enough to hold any
 * display-specific state data required by different display implementations.
 */
#define PIX_DISPLAY_CTX_SIZE 64

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Display callback function type for frame updates.
 * @ingroup PixelDisplay
 * @param display Pointer to the display that needs updating
 * @return true if the callback was successful, false on error
 */
typedef void (*pix_display_callback_t)(const struct pix_display_t *display);

/**
 * @brief Display context structure.
 * @ingroup PixelDisplay
 * @headerfile display.h runtime-pix/pix.h
 *
 * Contains the state for display operations.
 */
typedef struct pix_display_t {
  pix_frame_t frame; ///< Framebuffer containing pixel data for the display
  pix_display_callback_t
      callback;         ///< Optional callback function for frame updates
  uint64_t time_ms;     ///< Timestamp of last frame update in milliseconds
  uint64_t interval_ms; ///< Target frame interval in milliseconds
  void *userdata;       ///< User-defined data pointer passed to callbacks
  uint8_t ctx[PIX_DISPLAY_CTX_SIZE]; ///< Internal context buffer for
                                     ///< display-specific state
  bool (*lock)(struct pix_display_t
                   *display); ///< Function pointer to lock display for drawing
  bool (*unlock)(struct pix_display_t *display); ///< Function pointer to unlock
                                                 ///< display after drawing
} pix_display_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize a new SDL display with the specified format, size, and
 * interval.
 * @ingroup PixelDisplay
 * @param title Window title
 * @param size Display dimensions
 * @param format Pixel format
 * @param interval_ms Frame interval in milliseconds (0 = no limit)
 * @param callback Optional callback function called at the target interval
 * @param userdata User context pointer for the callback
 * @return Initialized display structure, or empty if initialization failed
 */
pix_display_t pix_sdl_display_init(const char *title, pix_size_t size,
                                   pix_format_t format, uint32_t interval_ms,
                                   pix_display_callback_t callback);

/**
 * @brief Finalize and free resources associated with an SDL display
 * @ingroup PixelDisplay
 */
bool pix_display_sdl_finalize(pix_display_t *display);

/**
 * @brief Perform operations in the runloop for the display.
 * @ingroup PixelDisplay
 */
bool pix_display_runloop(pix_display_t *display);

#ifdef __cplusplus
}
#endif
