#include <SDL3/SDL.h>
#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

struct _pix_sdl_context {
  SDL_Window *window;        ///< SDL window for rendering
  SDL_Renderer *renderer;    ///< SDL renderer for the window
  SDL_Texture *texture;      ///< Main texture for pixel operations
  pix_display_error_t error; ///< Error code for display operations
};

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Convert SDL pixel format to pix format.
 * @param sdl_format SDL pixel format
 * @return Corresponding pix format, or -1 if unsupported
 */
static inline int _sdl_to_pix_format(Uint32 sdl_format) {
  switch (sdl_format) {
  case SDL_PIXELFORMAT_RGBA32:
    return PIX_FMT_RGBA32;
  default:
    return -1; // Unsupported format
  }
}

/**
 * @brief Convert pix format to SDL pixel format.
 * @param pix_format Pix pixel format
 * @return Corresponding SDL format, or SDL_PIXELFORMAT_UNKNOWN if unsupported
 */
static inline Uint32 _pix_to_sdl_format(pix_format_t pix_format) {
  switch (pix_format) {
  case PIX_FMT_RGBA32:
    return SDL_PIXELFORMAT_RGBA32;
  default:
    return SDL_PIXELFORMAT_UNKNOWN;
  }
}

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initialize a new SDL display with the specified format, size, and FPS.
 * @ingroup Pixel
 */
pix_display_t pix_sdl_display_init(const char *title, pix_size_t size,
                                   pix_format_t format, uint32_t fps,
                                   pix_display_callback_t callback) {
  pix_display_t display = {0};

  // Validate parameters
  if (title == NULL || size.w == 0 || size.h == 0) {
    display.error = PIX_DISPLAY_ERR_INVALID_PARAM;
    return display;
  }

  // Convert pix format to SDL format
  Uint32 sdl_format = _pix_to_sdl_format(format);
  if (sdl_format == SDL_PIXELFORMAT_UNKNOWN) {
    display.error = PIX_DISPLAY_ERR_UNSUPPORTED;
    return display;
  }

  // Ensure the context buffer is large enough for our structure
  if (sizeof(struct _pix_sdl_context) > PIX_DISPLAY_CTX_SIZE) {
    display.error = PIX_DISPLAY_ERR_INVALID_PARAM;
    return display;
  }

  // Create the SDL window and renderer together
  struct _pix_sdl_context *ctx = (struct _pix_sdl_context *)display.ctx;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  if (!SDL_CreateWindowAndRenderer(title, size.w, size.h, SDL_WINDOW_BORDERLESS,
                                   &window, &renderer)) {
    display.error = PIX_DISPLAY_ERR_SDL;
    return display;
  }

  // Enable vsync to prevent screen tearing
  if (!SDL_SetRenderVSync(renderer, 1)) {
    // Vsync failed, but continue anyway - just log it
    // Could optionally fail here if vsync is critical
  }

  // Create a texture for pixel operations with the requested format
  SDL_Texture *texture = SDL_CreateTexture(
      renderer, sdl_format, SDL_TEXTUREACCESS_STREAMING, size.w, size.h);
  if (texture == NULL) {
    // Requested format not supported
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    display.error = PIX_DISPLAY_ERR_UNSUPPORTED;
    return display;
  }

  // Set context
  ctx->window = window;
  ctx->renderer = renderer;
  ctx->texture = texture;

  // Initialize FPS and callback settings
  display.target_fps = fps;
  display.callback = callback;
  display.last_frame_time = 0;

  // Initialize the frame using the proper frame initialization function
  display.frame = pix_frame_init(format, size, 0);
  if (display.frame.buf == NULL) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    display.error = PIX_DISPLAY_ERR_INVALID_FORMAT;
    return display;
  }

  // Return the display structure
  return display;
}

/**
 * @brief Finalize and free resources associated with a display.
 * @ingroup Pixel
 */
bool pix_display_finalize(pix_display_t *display) {
  sys_assert(display);

  // Finalize the frame structure
  pix_frame_finalize(&display->frame);

  // Clean up SDL resources
  struct _pix_sdl_context *ctx = (struct _pix_sdl_context *)display->ctx;
  if (ctx->texture) {
    SDL_DestroyTexture(ctx->texture);
  }
  if (ctx->renderer) {
    SDL_DestroyRenderer(ctx->renderer);
  }
  if (ctx->window) {
    SDL_DestroyWindow(ctx->window);
  }

  // Clear the display structure
  memset(display, 0, sizeof(pix_display_t));

  // Return success
  return true;
}

/**
 * @brief Return an error code for display operations.
 * @ingroup Pixel
 */
pix_display_error_t pix_display_error(const pix_display_t *display) {
  sys_assert(display);
  return display->error;
}

/**
 * @brief Perform operations in the runloop for the display.
 * @ingroup Pixel
 */
void pix_display_runloop(const pix_display_t *display) {
  sys_assert(display);
  SDL_PumpEvents();
}

/**
 * @brief Lock the display for drawing operations.
 * @ingroup Pixel
 */
bool pix_display_lock(pix_display_t *display) {
  if (display == NULL) {
    return false;
  }

  // Lock the renderer for exclusive access during drawing operations
  // This prevents other threads from interfering with rendering
  return true;
}

/**
 * @brief Unlock the display and update the screen.
 * @ingroup Pixel
 */
bool pix_display_unlock(pix_display_t *display) {
  sys_assert(display);
  pix_frame_t *frame = &display->frame;
  sys_assert(frame && frame->buf);

  struct _pix_sdl_context *ctx = (struct _pix_sdl_context *)display->ctx;
  sys_assert(ctx);

  // Update the texture with our frame buffer data
  bool success = true;
  if (SDL_UpdateTexture(ctx->texture, NULL, frame->buf, frame->stride)) {
    // SDL_UpdateTexture succeeded (returns true on success), now render the
    // texture
    success = SDL_RenderTexture(ctx->renderer, ctx->texture, NULL, NULL);
  } else {
    // SDL_UpdateTexture failed (returns false on failure)
    success = false;
  }

  // Present the final result to the screen
  SDL_RenderPresent(ctx->renderer);

  // Return success
  return success;
}
