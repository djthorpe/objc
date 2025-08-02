/**
 * @file pix.h
 * @brief Pixel display and frame management functions.
 * @defgroup Pixel
 * @ingroup System
 *
 * Managing pixel data, framebuffers, and drawing operations.
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
 * Frames do require a significant amount of memory, so they should be
 * used judiciously. On embedded systems, PIX_FMT_RGB332 provides limited
 * color depth but is memory efficient.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "display.h"
#include "frame.h"
#include "types.h"

///////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES

/**
 * @name Global Color and Point Constants
 * @ingroup Pixel
 * @{
 */
extern pix_point_t pix_zero_point; ///< Zero point (0,0)
extern pix_size_t pix_zero_size;   ///< Zero size (0,0)
extern pix_color_t pix_white;      ///< White (255,255,255,255) ARGB
extern pix_color_t pix_black;      ///< Black (0,0,0,255) ARGB
extern pix_color_t pix_red;        ///< Red (255,0,0,255) ARGB
extern pix_color_t pix_green;      ///< Green (0,255,0,255) ARGB
extern pix_color_t pix_blue;       ///< Blue (0,0,255,255) ARGB
extern pix_color_t pix_yellow;     ///< Yellow (255,255,0,255) ARGB
extern pix_color_t pix_cyan;       ///< Cyan (0,255,255,255) ARGB
extern pix_color_t pix_magenta;    ///< Magenta (255,0,255,255) ARGB
extern pix_color_t pix_gray;       ///< Gray (128,128,128,255) ARGB
extern pix_color_t pix_light_gray; ///< Light gray (192,192,192,255) ARGB
extern pix_color_t pix_dark_gray;  ///< Dark gray (64,64,64,255) ARGB
/** @} */

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes the pixel system on startup.
 * @ingroup Pixel
 *
 * This function must be called at the start of the program to initialize
 * the pixel environment.
 */
extern void pix_init(void);

/**
 * @brief Cleans up the pixel system on shutdown.
 * @ingroup Pixel
 *
 * This function should be called at the end of the program to perform any
 * necessary cleanup tasks.
 */
extern void pix_exit(void);
