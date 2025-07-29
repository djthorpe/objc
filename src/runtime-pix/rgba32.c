#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>
#include <stddef.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static void _pix_clear_rect_rgba32(pix_frame_t *frame, pix_color_t color,
                                   pix_point_t origin, pix_size_t size);
static pix_color_t _pix_get_rgba32(pix_frame_t *frame, pix_point_t origin);
static void _pix_set_rgba32(pix_frame_t *frame, pix_color_t color,
                            pix_point_t origin, pix_op_t op);
static size_t _pix_get_rect_rgba32(pix_frame_t *frame, pix_color_t *dst,
                                   pix_point_t origin, pix_size_t size);
static void _pix_set_rect_rgba32(pix_frame_t *frame, pix_color_t *src,
                                 pix_point_t origin, pix_size_t size,
                                 pix_op_t op);

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a new frame with the specified format and size.
 */
bool _pix_frame_init_rgba32(pix_frame_t *frame, pix_size_t size,
                            size_t alignment) {
  if (frame == NULL || size.w == 0 || size.h == 0) {
    return false; // Invalid parameters
  }
  if (alignment == 0) {
    alignment = sizeof(size_t); // Default alignment
  }

  // Set up the frame structure
  frame->format = PIX_FMT_RGBA32;
  frame->offset = pix_zero_point;
  frame->size = size;

  // Calculate stride with proper alignment
  size_t row_bytes = size.w * sizeof(pix_color_t);
  frame->stride =
      (row_bytes + alignment - 1) & ~(alignment - 1); // Align stride

  // Size of the buffer in bytes
  size_t buf_size = frame->stride * size.h;
  frame->buf = sys_malloc(buf_size);
  if (frame->buf == NULL) {
    return false; // Memory allocation failed
  } else {
    sys_memset(frame->buf, 0, buf_size); // Initialize buffer to zero
  }

  // set the function pointers
  frame->drawable = NULL;
  frame->clear_rect = _pix_clear_rect_rgba32;
  frame->get = _pix_get_rgba32;
  frame->set = _pix_set_rgba32;
  frame->get_rect = _pix_get_rect_rgba32;
  frame->set_rect = _pix_set_rect_rgba32;

  // Return success
  return true;
}

/**
 * @brief Finalize and free resources associated with a frame.
 */
bool _pix_frame_finalize_rgba32(pix_frame_t *frame) {
  if (frame == NULL || frame->buf == NULL) {
    return false; // Nothing to finalize
  }

  // Release the buffer memory
  sys_free(frame->buf);
  frame->buf = NULL;
  frame->size = pix_zero_size;
  frame->offset = pix_zero_point;

  // NULLify the function pointers
  frame->drawable = NULL;
  frame->clear_rect = NULL;
  frame->get = NULL;
  frame->set = NULL;
  frame->get_rect = NULL;
  frame->set_rect = NULL;

  // Return success
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

static void _pix_clear_rect_rgba32(pix_frame_t *frame, pix_color_t color,
                                   pix_point_t origin, pix_size_t size) {
  // Check drawable state
  if (frame == NULL || frame->buf == NULL) {
    return;
  } else if (frame->drawable && frame->drawable() == false) {
    return; // Frame is not drawable
  }

  // Suppress unused parameter warnings
  (void)color;
  (void)origin;
  (void)size;

  // TODO
}

static pix_color_t _pix_get_rgba32(pix_frame_t *frame, pix_point_t origin) {
  // Check drawable state
  if (frame == NULL || frame->buf == NULL) {
    return 0; // Return transparent color if not drawable
  } else if (frame->drawable && frame->drawable() == false) {
    return 0; // Return transparent color if not drawable
  }

  // Suppress unused parameter warnings
  (void)origin;

  // TODO
  return 0;
}

static void _pix_set_rgba32(pix_frame_t *frame, pix_color_t color,
                            pix_point_t origin, pix_op_t op) {
  // Check drawable state
  if (frame == NULL || frame->buf == NULL) {
    return;
  } else if (frame->drawable && frame->drawable() == false) {
    return;
  }

  // Bounds checking
  if (origin.x < 0 || origin.y < 0 || origin.x >= frame->size.w ||
      origin.y >= frame->size.h) {
    return; // Out of bounds
  }

  // Suppress unused parameter warnings
  (void)color;
  (void)op;

  // TODO
}

static size_t _pix_get_rect_rgba32(pix_frame_t *frame, pix_color_t *dst,
                                   pix_point_t origin, pix_size_t size) {
  // Check drawable state
  if (frame == NULL || frame->buf == NULL) {
    return 0;
  } else if (frame->drawable && frame->drawable() == false) {
    return 0;
  }

  // Suppress unused parameter warnings
  (void)dst;
  (void)origin;
  (void)size;

  // TODO
  return 0;
}

static void _pix_set_rect_rgba32(pix_frame_t *frame, pix_color_t *src,
                                 pix_point_t origin, pix_size_t size,
                                 pix_op_t op) {
  // Check drawable state
  if (frame == NULL || frame->buf == NULL) {
    return;
  } else if (frame->drawable && frame->drawable() == false) {
    return;
  }

  // Suppress unused parameter warnings
  (void)src;
  (void)origin;
  (void)size;
  (void)op;

  // TODO
}
