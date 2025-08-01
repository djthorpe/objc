/**
 * @file frame.h
 * @brief Frame functions.
 * @defgroup PixelFrame Pixel Frames
 * @ingroup Pixel
 *
 * Functions for pixel frames
 */
#pragma once
#include "types.h"

/**
 * @brief Frame structure containing pixel data and drawing operations.
 * @ingroup PixelFrame
 * @headerfile frame.h runtime-pix/pix.h
 *
 * A frame represents a rectangular region of pixels with associated
 * drawing functions. The frame is not thread-safe for performance
 * reasons - callers must provide their own synchronization if needed.
 */
typedef struct pix_frame_t {
  pix_point_t offset;  ///< Offset (in pixels) to the origin within the buffer
  pix_size_t size;     ///< Size of the buffer in pixels
  pix_format_t format; ///< Pixel format of the buffer
  void *buf;           ///< Raw buffer holding pixel data
  size_t stride;       ///< Bytes per scanline

  /**
   * @brief Check if the framebuffer is currently drawable.
   * @return true if the framebuffer can be safely drawn to, false otherwise.
   *
   * This function is set by windowing or device systems that may be
   * concurrently updating the rendered surface. If it is NULL,
   * the frame is always considered drawable.
   */
  bool (*drawable)(void);

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
  void (*clear_rect)(struct pix_frame_t *frame, pix_color_t color,
                     pix_point_t origin, pix_size_t size);

  /**
   * @brief Get a single pixel value at the specified point.
   * @param frame Pointer to the frame to operate on
   * @param origin Coordinates of the pixel to retrieve
   * @return Color value of the pixel in RGBA32 format
   */
  pix_color_t (*get)(struct pix_frame_t *frame, pix_point_t origin);

  /**
   * @brief Set a single pixel value at the specified point.
   * @param frame Pointer to the frame to operate on
   * @param color Color value to set (RGBA32 format)
   * @param origin Coordinates of the pixel to set
   * @param op Pixel operation to perform (e.g., set, blend, etc.)
   */
  void (*set)(struct pix_frame_t *frame, pix_color_t color, pix_point_t origin,
              pix_op_t op);

  /**
   * @brief Get a rectangular region of pixels.
   * @param frame Pointer to the frame to operate on
   * @param dst Destination buffer for pixel data (can be NULL for size query)
   * @param origin Top-left corner of the rectangle to retrieve
   * @param size Dimensions of the rectangle to retrieve
   * @return Size in bytes of the retrieved data, or required buffer size if dst
   * is NULL
   * @details If dst is NULL, only returns the expected buffer size needed.
   */
  size_t (*get_rect)(struct pix_frame_t *frame, pix_color_t *dst,
                     pix_point_t origin, pix_size_t size);

  /**
   * @brief Set a rectangular region of pixels from RGBA32 source data.
   * @param frame Pointer to the frame to operate on
   * @param src Source buffer containing RGBA32 pixel data
   * @param origin Top-left corner where the rectangle should be placed
   * @param size Dimensions of the rectangle to set
   * @param op Pixel operation to perform for each pixel
   */
  void (*set_rect)(struct pix_frame_t *frame, pix_color_t *src,
                   pix_point_t origin, pix_size_t size, pix_op_t op);
} pix_frame_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a new framebuffer with the specified format and size.
 * @ingroup PixelFrame
 * @param format Pixel format for the framebuffer
 * @param size Dimensions of the framebuffer in pixels
 * @param alignment Memory alignment requirement for the buffer (0 for default)
 * @return Initialized framebuffer structure with allocated buffer
 *
 * Returns a framebuffer structure with an allocated buffer of the specified
 * size and format. The buffer is aligned according to the specified alignment.
 * If alignment is 0, the default alignment for the platform is used, which is
 * usually sizeof(size_t).
 *
 * If the allocation fails, or there are other errors, the function returns a
 * zero-sized frame. The caller is responsible for checking the return value.
 * Otherwise, the methods for manipulating the frame will be populated according
 * to the specified pixel format.
 *
 * The "drawable" function will be set to NULL by default, meaning the
 * framebuffer is always considered drawable. Callers can set this function
 * to a custom implementation if needed, such as for windowing systems that
 * may update the framebuffer concurrently.
 */
pix_frame_t pix_frame_init(pix_format_t format, pix_size_t size,
                           size_t alignment);

/**
 * @brief Finalize and free resources associated with a framebuffer.
 * @ingroup PixelFrame
 * @param frame Pointer to the framebuffer to finalize
 * @return true if successfully finalized, false on error.
 *
 * This function releases the memory allocated for the framebuffer's pixel
 * buffer and resets the framebuffer structure. The structure cannot be
 * reused after finalization.
 */
bool pix_frame_finalize(pix_frame_t *frame);

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

/**
 * @brief Clear a rectangular region to a solid color.
 * @ingroup PixelFrame
 * @param frame Pointer to the frame to operate on
 * @param color Color to fill the rectangle with
 * @param origin Top-left corner of the rectangle to clear
 * @param size Dimensions of the rectangle to clear. If zero, clears the
 * entire frame.
 *
 * Completely overwrites existing pixel data in the specified region.
 */
bool pix_frame_clear_rect(pix_frame_t *frame, pix_color_t color,
                          pix_point_t origin, pix_size_t size);