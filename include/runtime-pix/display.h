/**
 * @file display.h
 * @brief Display functions.
 * @ingroup Pixels
 *
 * Functions for pixel displays
 */
#pragma once
#include "types.h"
#include <stdbool.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Forward declarations to avoid circular dependency
struct pix_frame_t;

#define PIX_DISPLAY_CTX_SIZE 64

/**
 * @brief Display callback function type for frame updates.
 * @ingroup Pixels
 * @param display Pointer to the display that needs updating
 * @return true if the callback was successful, false on error
 */
typedef bool (*pix_display_callback_t)(const struct pix_display_t *display);

/**
 * @brief Error codes for display operations.
 * @ingroup Pixels
 */
typedef enum {
  PIX_DISPLAY_SUCCESS = 0,        ///< Operation successful
  PIX_DISPLAY_ERR_INVALID_PARAM,  ///< Invalid parameter
  PIX_DISPLAY_ERR_UNSUPPORTED,    ///< Unsupported pixel format
  PIX_DISPLAY_ERR_INVALID_FORMAT, ///< Invalid format for frame initialization
  PIX_DISPLAY_ERR_SDL,            ///< SDL error
} pix_display_error_t;

// Forward declaration of display structure (defined in pix.h)
typedef struct pix_display_t pix_display_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize a new SDL display with the specified format, size, and FPS.
 * @ingroup Pixel
 * @param title Window title
 * @param size Display dimensions
 * @param format Pixel format
 * @param fps Target frames per second (0 = no limit)
 * @param callback Optional callback function called at the target FPS
 */
pix_display_t pix_sdl_display_init(const char *title, pix_size_t size,
                                   pix_format_t format, uint32_t fps,
                                   pix_display_callback_t callback);

/**
 * @brief Finalize and free resources associated with a framebuffer.
 * @ingroup Pixel
 */
bool pix_display_finalize(pix_display_t *display);

/**
 * @brief Perform operations in the runloop for the display.
 * @ingroup Pixel
 */
void pix_display_runloop(const pix_display_t *display);

/**
 * @brief Return an error code for the display.
 * @ingroup Pixel
 */
pix_display_error_t pix_display_error(const pix_display_t *display);

/**
 * @brief Lock the display for drawing operations.
 * @ingroup Pixel
 * @param display Pointer to the display to lock
 * @return true if successfully locked, false on error
 *
 * This function must be called before performing any drawing operations
 * on the display's frame buffer. It ensures exclusive access and may
 * prepare the underlying graphics system for pixel operations.
 */
bool pix_display_lock(pix_display_t *display);

/**
 * @brief Unlock the display and update the screen.
 * @ingroup Pixel
 * @param display Pointer to the display to unlock
 * @return true if successfully unlocked and updated, false on error
 *
 * This function must be called after completing drawing operations.
 * It releases the lock and may trigger a screen update to show
 * the changes made to the frame buffer.
 */
bool pix_display_unlock(pix_display_t *display);

#ifdef __cplusplus
}
#endif
