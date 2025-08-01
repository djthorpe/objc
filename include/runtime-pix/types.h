/**
 * @file types.h
 * @brief Common pixel types and structures.
 * @ingroup Pixel
 *
 * Shared type definitions used across the pixel library.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Point structure representing X,Y coordinates.
 * @ingroup Pixel
 */
typedef struct {
  int16_t x; ///< X coordinate
  int16_t y; ///< Y coordinate
} pix_point_t;

/**
 * @brief Size structure representing width and height dimensions.
 * @ingroup Pixel
 */
typedef struct {
  uint16_t w; ///< Width in pixels
  uint16_t h; ///< Height in pixels
} pix_size_t;

/**
 * @brief Pixel format enumeration defining color depth and layout.
 * @ingroup Pixel
 */
typedef enum {
  PIX_FMT_RGBA32, ///< 32-bit RGBA format with alpha channel
  PIX_FMT_RGB332, ///< 8-bit RGB format (3-3-2 bits) without alpha
  PIX_FMT_RGB565, ///< 16-bit RGB format (5-6-5 bits) without alpha
  PIX_FMT_GREY1,  ///< 1-bit grayscale format (alpha mask)
} pix_format_t;

/**
 * @brief Pixel operation types for drawing operations.
 * @ingroup Pixel
 */
typedef enum {
  PIX_SET ///< Set pixel operation
} pix_op_t;

/**
 * @brief Color value type for pixel operations.
 * @ingroup Pixel
 * @details Represents a color value, typically in RGBA format depending on the
 * pixel format.
 */
typedef uint32_t pix_color_t;
