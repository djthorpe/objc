#include "rgba32.h"
#include <runtime-pix/pix.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

pix_point_t pix_zero_point = {0};        ///< Zero point (0,0)
pix_size_t pix_zero_size = {0};          ///< Zero size (0,0)
pix_color_t pix_white = 0xFFFFFFFF;      ///< White (255,255,255,255)
pix_color_t pix_black = 0x000000FF;      ///< Black (0,0,0,255)
pix_color_t pix_red = 0xFF0000FF;        ///< Red (255,0,0,255)
pix_color_t pix_green = 0x00FF00FF;      ///< Green (0,255,0,255)
pix_color_t pix_blue = 0x0000FFFF;       ///< Blue (0,0,255,255)
pix_color_t pix_yellow = 0xFFFF00FF;     ///< Yellow (255,255,0,255)
pix_color_t pix_cyan = 0x00FFFFFF;       ///< Cyan (0,255,255,255)
pix_color_t pix_magenta = 0xFF00FFFF;    ///< Magenta (255,0,255,255)
pix_color_t pix_gray = 0x808080FF;       ///< Gray (128,128,128,255)
pix_color_t pix_light_gray = 0xC0C0C0FF; ///< Light gray (192,192,192,255)
pix_color_t pix_dark_gray = 0x404040FF;  ///< Dark gray (64,64,64,255)

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

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
