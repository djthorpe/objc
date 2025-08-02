#include <runtime-pix/pix.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <tests/tests.h>
#ifdef SUPPORTED_SDL3
#include <SDL3/SDL.h>
#endif

// Global variables for the drawing callback
static int frame_counter = 0;

// Callback function that does the drawing with smooth color fading
void draw_callback(const struct pix_display_t *display) {
  // Cast away const since we need to draw (this is safe in our context)
  pix_display_t *mutable_display = (pix_display_t *)display;

  // Each color phase lasts 10 seconds at 25 FPS = 250 frames per color
  const int frames_per_color = 250;
  const int total_phases = 3; // red, blue, green

  // Calculate which color phase we're in and position within that phase
  int phase = (frame_counter / frames_per_color) % total_phases;
  int frame_in_phase = frame_counter % frames_per_color;

  // Calculate fade intensity (0.0 to 1.0, fading in first half, out second
  // half)
  float intensity;
  if (frame_in_phase < frames_per_color / 2) {
    // Fade in: 0 to 1
    intensity = (float)frame_in_phase / (frames_per_color / 2);
  } else {
    // Fade out: 1 to 0
    intensity = 1.0f - (float)(frame_in_phase - frames_per_color / 2) /
                           (frames_per_color / 2);
  }

  // Ensure intensity stays in valid range
  if (intensity < 0.0f)
    intensity = 0.0f;
  if (intensity > 1.0f)
    intensity = 1.0f;

  // Calculate color components based on phase and intensity
  uint8_t alpha = 255;
  uint8_t red = 0, green = 0, blue = 0;

  switch (phase) {
  case 0: // Red phase
    red = (uint8_t)(255 * intensity);
    break;
  case 1: // Blue phase
    blue = (uint8_t)(255 * intensity);
    break;
  case 2: // Green phase
    green = (uint8_t)(255 * intensity);
    break;
  }

  // Create ARGB color (format: 0xAARRGGBB, but with swapped R/B for SDL)
  pix_color_t current_color = (alpha << 24) | (blue << 16) | (green << 8) | red;

  // Clear the frame with the current color (runloop handles locking/unlocking)
  pix_frame_clear_rect(&mutable_display->frame, current_color, pix_zero_point,
                       pix_zero_size);

  frame_counter++;
}

int test_pix_01(void);

int main(void) { return TestPixMain("test_pix_01", test_pix_01); }

int test_pix_01(void) {
  // Test display creation and finalization
  pix_size_t size = {230, 230};

  // Test with valid parameters - 40ms interval (25 FPS) with drawing callback
  printf("Creating SDL display with callback...\n");
  pix_display_t display = pix_sdl_display_init(
      "Test Window", size, PIX_FMT_RGBA32, 40, draw_callback);

  printf("Display created successfully\n");

  // Run the display loop for 30 seconds at 25 FPS (750 frames total)
  // 10 seconds red + 10 seconds blue + 10 seconds green
  printf("Running color fade demo for 30 seconds...\n");
  int frames_drawn = 0;
  int target_frames = 750; // 30 seconds at 25 FPS

  while (frames_drawn < target_frames) {
    if (pix_display_runloop(&display)) {
      frames_drawn++;
    }
    sys_sleep(5); // Small sleep to prevent busy waiting
  }

  // Test finalization
  printf("Finalizing display...\n");
  bool finalize_result = pix_display_sdl_finalize(&display);
  test_assert(finalize_result == true);

  printf("Test completed successfully - drew %d frames in 30 seconds\n",
         frames_drawn);

  return 0;
}
