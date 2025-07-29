/**
 * @file pix.h
 * @brief Pixel and framebuffer management functions.
 * @defgroup Pixel Pixel Management
 *
 * This module declares types and functions for managing pixel data and
 * framebuffers, including pixel formats, drawing operations, and framebuffer
 * initialization and finalization.
 *
 * Pixels are represented in various formats, but externally they are
 * typically represented in RGBA32 format (a uint32_t). If a frame is
 * "drawable", it means it can be safely accessed and modified by drawing
 * operations. The drawable state can be set by windowing or device systems
 * that may update the rendered surface concurrently.
 *
 * Sizes are represented as structures with uint16_t fields
 * for dimensions, allowing for values between 0 and 65535. Points are
 * represented as structures with int16_t fields for coordinates, allowing
 * for values between -32768 and 32767.
 *
 * Frames do require a signiifcant amount of memory, so they should be
 * used judiciously. On embedded systems, PIX_FMT_RGB332 provides limited
 * color depth but is memory efficient.
 */
#pragma once

/**
 * @brief Pixel operation types for drawing operations.
 */
typedef enum {
  PIX_SET ///< Set pixel operation
} pix_op_t;

/**
 * @brief Color value type for pixel operations.
 * @details Represents a color value, typically in RGBA format depending on the
 * pixel format.
 */
typedef uint32_t pix_color_t;

/**
 * @brief Point structure representing X,Y coordinates.
 */
typedef struct {
  int16_t x; ///< X coordinate
  int16_t y; ///< Y coordinate
} pix_point_t;

/**
 * @brief Size structure representing width and height dimensions.
 */
typedef struct {
  uint16_t w; ///< Width in pixels
  uint16_t h; ///< Height in pixels
} pix_size_t;

/**
 * @brief Pixel format enumeration defining color depth and layout.
 */
typedef enum {
  PIX_FMT_RGBA32, ///< 32-bit RGBA format with alpha channel
  PIX_FMT_RGB332, ///< 8-bit RGB format (3-3-2 bits) without alpha
  PIX_FMT_RGB565, ///< 16-bit RGB format (5-6-5 bits) without alpha
  PIX_FMT_GREY1,  ///< 1-bit grayscale format (alpha mask)
} pix_format_t;

/**
 * @brief Frame structure containing pixel data and drawing operations.
 *
 * A frame represents a rectangular region of pixels with associated
 * drawing functions. The frame is not thread-safe for performance
 * reasons - callers must provide their own synchronization if needed.
 */
typedef struct {
  pix_point_t offset;  ///< Offset (in pixels) to the origin within the buffer
  pix_size_t size;     ///< Size of the buffer in pixels
  pix_format_t format; ///< Pixel format of the buffer
  void *buf;           ///< Raw buffer holding pixel data
  size_t stride;       ///< Bytes per scanline
  size_t buf_size;     ///< Total size of the buffer in bytes

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
   * @param color Color to fill the rectangle with
   * @param origin Top-left corner of the rectangle to clear
   * @param size Dimensions of the rectangle to clear. If zero, clears the
   * entire frame.
   *
   * Completely overwrites existing pixel data in the specified region.
   */
  void (*clear_rect)(pix_color_t color, pix_point_t origin, pix_size_t size);

  /**
   * @brief Get a single pixel value at the specified point.
   * @param origin Coordinates of the pixel to retrieve
   * @return Color value of the pixel in RGBA32 format
   */
  pix_color_t (*get)(pix_point_t origin);

  /**
   * @brief Set a single pixel value at the specified point.
   * @param color Color value to set (RGBA32 format)
   * @param origin Coordinates of the pixel to set
   * @param op Pixel operation to perform (e.g., set, blend, etc.)
   * @return true if the pixel was successfully set, false otherwise
   */
  bool (*set)(pix_color_t color, pix_point_t origin, pix_op_t op);

  /**
   * @brief Get a rectangular region of pixels.
   * @param dst Destination buffer for pixel data (can be NULL for size query)
   * @param origin Top-left corner of the rectangle to retrieve
   * @param size Dimensions of the rectangle to retrieve
   * @return Size in bytes of the retrieved data, or required buffer size if dst
   * is NULL
   * @details If dst is NULL, only returns the expected buffer size needed.
   */
  size_t (*get_rect)(pix_color_t *dst, pix_point_t origin, pix_size_t size);

  /**
   * @brief Set a rectangular region of pixels from RGBA32 source data.
   * @param src Source buffer containing RGBA32 pixel data
   * @param origin Top-left corner where the rectangle should be placed
   * @param size Dimensions of the rectangle to set
   * @param op Pixel operation to perform for each pixel
   */
  void (*set_rect)(pix_color_t *src, pix_point_t origin, pix_size_t size,
                   pix_op_t op);
} pix_frame_t;

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

extern pix_point_t pix_zero_point; ///< Zero point (0,0)
extern pix_size_t pix_zero_size;   ///< Zero size (0,0)
extern pix_color_t pix_white;      ///< White (255,255,255,255)
extern pix_color_t pix_black;      ///< Black (0,0,0,255)
extern pix_color_t pix_red;        ///< Red (255,0,0,255)
extern pix_color_t pix_green;      ///< Green (0,255,0,255)
extern pix_color_t pix_blue;       ///< Blue (0,0,255,255)
extern pix_color_t pix_yellow;     ///< Yellow (255,255,0,255)
extern pix_color_t pix_cyan;       ///< Cyan (0,255,255,255)
extern pix_color_t pix_magenta;    ///< Magenta (255,0,255,255)
extern pix_color_t pix_gray;       ///< Gray (128,128,128,255)
extern pix_color_t pix_light_gray; ///< Light gray (192,192,192,255)
extern pix_color_t pix_dark_gray;  ///< Dark gray (64,64,64,255)

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a new framebuffer with the specified format and size.
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
 * @param frame Pointer to the framebuffer to finalize
 * @return true if successfully finalized, false on error.
 *
 * This function releases the memory allocated for the framebuffer's pixel
 * buffer and resets the framebuffer structure. The structure cannot be
 * reused after finalization.
 */
bool pix_frame_finalize(pix_frame_t *frame);
