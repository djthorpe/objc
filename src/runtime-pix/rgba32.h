#include <runtime-pix/pix.h>

bool _pix_frame_init_rgba32(pix_frame_t *frame, pix_size_t size,
                            size_t alignment);

bool _pix_frame_finalize_rgba32(pix_frame_t *frame);