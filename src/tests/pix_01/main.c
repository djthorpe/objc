#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <tests/tests.h>
#ifdef SUPPORTED_SDL3
#include <SDL3/SDL.h>
#endif

int test_pix_01(void);

int main(void) { return TestPixMain("test_pix_01", test_pix_01); }

int test_pix_01(void) {
  // Test display creation and finalization
  pix_size_t size = {230, 230};

  // Test with valid parameters
  printf("Creating SDL display...\n");
  pix_display_t display =
      pix_sdl_display_init("Test Window", size, PIX_FMT_RGBA32);

  // Check that display creation was successful
  pix_display_error_t error = pix_display_error(&display);
  printf("Display error code: %d\n", error);

  if (error != PIX_DISPLAY_SUCCESS) {
#ifdef SUPPORTED_SDL3
    printf("SDL Error: %s\n", SDL_GetError());
#endif
    return 1; // Exit early on failure
  }

  // Sleep for 3 seconds to show the window with event pumping
  pix_color_t colors[] = {pix_red,    pix_green, pix_blue,
                          pix_yellow, pix_cyan,  pix_magenta};
  int color_count = sizeof(colors) / sizeof(colors[0]);

  for (int i = 0; i < 3000; i++) {
    // Lock the display for drawing
    if (!pix_display_lock(&display)) {
      printf("Warning: Failed to lock display at iteration %d\n", i);
      continue;
    }

    // Clear the frame with a cycling color
    pix_color_t current_color = colors[i % color_count];
    bool clear_result = pix_frame_clear_rect(&display.frame, current_color,
                                             pix_zero_point, pix_zero_size);
    if (!clear_result) {
      printf("Warning: Failed to clear frame at iteration %d\n", i);
    }

    // Unlock the display to update the screen
    if (!pix_display_unlock(&display)) {
      printf("Warning: Failed to unlock display at iteration %d\n", i);
    }

    pix_display_runloop(&display);
    sys_sleep(5); // 5 chunks
  }

  // Test finalization
  bool finalize_result = pix_display_finalize(&display);
  test_assert(finalize_result == true);

  return 0;
}
