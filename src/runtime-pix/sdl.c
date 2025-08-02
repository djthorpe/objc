#include <SDL3/SDL.h>
#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

struct _pix_sdl_context {
  SDL_Window *window;     ///< SDL window for rendering
  SDL_Renderer *renderer; ///< SDL renderer for the window
  SDL_Texture *texture;   ///< Main texture for pixel operations
};

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static bool _pix_sdl_display_lock(pix_display_t *display);
static bool _pix_sdl_display_unlock(pix_display_t *display);

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
 * @brief Initialize a new SDL display with the specified format, size, and
 * interval.
 */
pix_display_t pix_sdl_display_init(const char *title, pix_size_t size,
                                   pix_format_t format, uint32_t interval_ms,
                                   pix_display_callback_t callback) {
  pix_display_t display = {0};

  // Validate parameters
  if (title == NULL || size.w == 0 || size.h == 0) {
#ifdef DEBUG
    sys_puts("pix_display_t: invalid parameters\n");
#endif
    return display;
  }

  // Convert pix format to SDL format
  Uint32 sdl_format = _pix_to_sdl_format(format);
  if (sdl_format == SDL_PIXELFORMAT_UNKNOWN) {
#ifdef DEBUG
    sys_puts("pix_display_t: invalid pixel format\n");
#endif
    return display;
  }

  // Ensure the context buffer is large enough for our structure
  if (sizeof(struct _pix_sdl_context) > PIX_DISPLAY_CTX_SIZE) {
#ifdef DEBUG
    sys_puts("pix_display_t: invalid parameters\n");
#endif
    return display;
  }

  // Create the SDL window and renderer together
  struct _pix_sdl_context *ctx = (struct _pix_sdl_context *)display.ctx;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  if (!SDL_CreateWindowAndRenderer(title, size.w, size.h, SDL_WINDOW_RESIZABLE,
                                   &window, &renderer)) {
#ifdef DEBUG
    sys_puts(SDL_GetError());
#endif
    return display;
  }

  // Enable vsync to prevent screen tearing
  if (!SDL_SetRenderVSync(renderer, 1)) {
#ifdef DEBUG
    sys_puts(SDL_GetError());
#endif
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return display;
  }

  // Clear the renderer to black initially
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  // Create a texture for pixel operations with the requested format
  SDL_Texture *texture = SDL_CreateTexture(
      renderer, sdl_format, SDL_TEXTUREACCESS_STREAMING, size.w, size.h);
  if (texture == NULL) {
#ifdef DEBUG
    sys_puts(SDL_GetError());
#endif
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return display;
  }

  // Set context
  ctx->window = window;
  ctx->renderer = renderer;
  ctx->texture = texture;

  // Initialize the frame using the proper frame initialization function
  display.frame = pix_frame_init(format, size, 0);
  if (display.frame.buf == NULL) {
#ifdef DEBUG
    sys_puts("pix_display_t: unable to initialize frame buffer\n");
#endif
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return display;
  } else {
    // Clear the frame buffer to black to ensure clean initial state
    size_t total_bytes = display.frame.size.w * display.frame.size.h *
                         4; // RGBA32 = 4 bytes per pixel
    memset(display.frame.buf, 0, total_bytes);
    // TODO: Set the 'drawable' function pointer
  }

  // Initialize the display structure
  display.callback = callback;
  display.time_ms = 0;
  display.interval_ms = interval_ms;
  display.userdata = NULL; // Store context pointer in userdata
  display.lock = _pix_sdl_display_lock;
  display.unlock = _pix_sdl_display_unlock;

  // Return the display structure
  return display;
}

/**
 * @brief Finalize and free resources associated with a SDL display.
 * @ingroup Pixel
 */
bool pix_display_sdl_finalize(pix_display_t *display) {
  sys_assert(display);
  sys_assert(display->frame.buf);

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
 * @brief Lock the display for drawing operations.
 * @ingroup Pixel
 */
static bool _pix_sdl_display_lock(pix_display_t *display) {
  sys_assert(display);

  struct _pix_sdl_context *ctx = (struct _pix_sdl_context *)display->ctx;
  sys_assert(ctx);

  // For SDL, we don't need to do much for locking, but we can ensure
  // the renderer is ready and clear any previous state
  return ctx->renderer != NULL;
}

/**
 * @brief Unlock the display and update the screen.
 * @ingroup Pixel
 */
static bool _pix_sdl_display_unlock(pix_display_t *display) {
  sys_assert(display);
  pix_frame_t *frame = &display->frame;
  sys_assert(frame && frame->buf);

  struct _pix_sdl_context *ctx = (struct _pix_sdl_context *)display->ctx;
  sys_assert(ctx);

  // Update the texture with our frame buffer data
  if (!SDL_UpdateTexture(ctx->texture, NULL, frame->buf, frame->stride)) {
#ifdef DEBUG
    sys_puts("SDL_UpdateTexture failed\n");
#endif
    return false;
  }

  // Render the texture to the back buffer
  if (!SDL_RenderTexture(ctx->renderer, ctx->texture, NULL, NULL)) {
#ifdef DEBUG
    sys_puts("SDL_RenderTexture failed\n");
#endif
    return false;
  }

  // Present the final result to the screen
  SDL_RenderPresent(ctx->renderer);

  // Return success
  return true;
}
