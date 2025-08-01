#include "rgba32.h"
#include <runtime-pix/pix.h>
#include <runtime-sys/assert.h>
#ifdef SUPPORTED_SDL3
#include <SDL3/SDL.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

pix_point_t pix_zero_point = {0};   ///< Zero point (0,0)
pix_size_t pix_zero_size = {0};     ///< Zero size (0,0)
pix_color_t pix_white = 0xFFFFFFFF; ///< White (255,255,255,255) ARGB
pix_color_t pix_black = 0xFF000000; ///< Black (0,0,0,255) ARGB
pix_color_t pix_red = 0xFF0000FF; ///< Red (255,0,0,255) ARGB - swapped for SDL
pix_color_t pix_green = 0xFF00FF00; ///< Green (0,255,0,255) ARGB
pix_color_t pix_blue =
    0xFFFF0000; ///< Blue (0,0,255,255) ARGB - swapped for SDL
pix_color_t pix_yellow =
    0xFF00FFFF; ///< Yellow (255,255,0,255) ARGB - swapped for SDL
pix_color_t pix_cyan =
    0xFFFFFF00; ///< Cyan (0,255,255,255) ARGB - swapped for SDL
pix_color_t pix_magenta =
    0xFFFF00FF; ///< Magenta (255,0,255,255) ARGB - unchanged (red+blue)
pix_color_t pix_gray = 0xFF808080;       ///< Gray (128,128,128,255) ARGB
pix_color_t pix_light_gray = 0xFFC0C0C0; ///< Light gray (192,192,192,255) ARGB
pix_color_t pix_dark_gray = 0xFF404040;  ///< Dark gray (64,64,64,255) ARGB

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the pixel system on startup.
 * @ingroup Pixel
 *
 * This function must be called at the start of the program to initialize
 * the pixel environment.
 */
void pix_init(void) {
#ifdef SUPPORTED_SDL3
  bool success = SDL_Init(SDL_INIT_VIDEO);
  if (!success) {
    sys_panicf("Failed to initialize SDL: %s", SDL_GetError());
  }
#endif
}

/**
 * @brief Cleans up the pixel system on shutdown.
 */
void pix_exit(void) {
#ifdef SUPPORTED_SDL3
  SDL_Quit();
#endif
}

/**
 * @brief Initialize a new frame with the specified format and size.
 */
pix_frame_t pix_frame_init(pix_format_t format, pix_size_t size,
                           size_t alignment) {
  pix_frame_t frame = {0};

  // Initialize the frame structure based on the pixel format
  switch (format) {
  case PIX_FMT_RGBA32:
    if (_pix_frame_init_rgba32(&frame, size, alignment) == false) {
      return (pix_frame_t){0}; // Initialization failed, return empty frame
    }
    break;
  default:
    // Unsupported format, return zero-sized frame
  }

  // Return the frame
  return frame;
}

/**
 * @brief Finalize and free resources associated with a frame
 */
bool pix_frame_finalize(pix_frame_t *frame) {
  if (frame == NULL || frame->buf == NULL) {
    return false; // Nothing to finalize
  }
  switch (frame->format) {
  case PIX_FMT_RGBA32:
    return _pix_frame_finalize_rgba32(frame);
  default:
    // Unsupported format
    return false;
  }
}

/**
 * @brief Clear a rectangular region to a solid color.
 * @param frame Pointer to the frame to operate on
 * @param color Color to fill the rectangle with
 * @param origin Top-left corner of the rectangle to clear
 * @param size Dimensions of the rectangle to clear. If zero, clears the
 * entire frame.
 *
 * Completely overwrites existing pixel data in the specified region.
 */
bool pix_frame_clear_rect(pix_frame_t *frame, pix_color_t color,
                          pix_point_t origin, pix_size_t size) {
  sys_assert(frame);
  if (frame->drawable && frame->drawable() == false) {
    return false; // Frame is not drawable
  }
  if (frame->clear_rect) {
    frame->clear_rect(frame, color, origin, size);
    return true; // Clear operation successful
  } else {
    return false; // No clear_rect function defined
  }
}
