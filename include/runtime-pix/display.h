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
  pix_frame_t frame;                 // Framebuffer for the display
  pix_display_callback_t callback;   // Optional callback for frame updates
  uint64_t time_ms;                  // Last frame timestamp (milliseconds)
  uint64_t interval_ms;              // Frame interval (milliseconds)
  void *userdata;                    // User data pointer for callbacks
  uint8_t ctx[PIX_DISPLAY_CTX_SIZE]; // Context buffer large enough for any
                                     // display context
  bool (*lock)(struct pix_display_t *display); ///< Lock the display for drawing
  bool (*unlock)(
      struct pix_display_t *display); ///< Unlock the display after drawing
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
