#ifdef SUPPORTED_SDL3
#include <SDL3/SDL.h>
#endif
#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>

/**
 * @brief Perform operations in the runloop for the display.
 * @ingroup Pixel
 */
bool pix_display_runloop(pix_display_t *display) {
  sys_assert(display);
#ifdef SUPPORTED_SDL3
  SDL_PumpEvents();
#endif

  // Check if enough time has passed for the next frame
#ifdef SUPPORTED_SDL3
  uint64_t now = SDL_GetTicks();
#else
  uint64_t now = 0;
#endif
  if (display->time_ms != 0 && display->time_ms + display->interval_ms > now) {
    // Not enough time has passed, skip this frame
    return false;
  }

  // Update the frame time before doing any work
  display->time_ms = now;

  // Lock the display for drawing operations
  if (display->lock &&
      display->lock((struct pix_display_t *)display) == false) {
    return false; // Failed to lock display for drawing
  }

  // Callback for frame updates
  if (display->callback) {
    display->callback((const struct pix_display_t *)display);
  }

  // Unlock the display after drawing
  if (display->unlock) {
    return display->unlock((struct pix_display_t *)display);
  } else {
    return true;
  }
}
