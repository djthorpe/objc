#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_fs_01(void) {
  size_t size = 800;

  // Create a memory-backed file system volume
  fs_volume_t *volume = fs_vol_init_memory(size);
  test_assert(volume);
  size_t volume_size = fs_vol_size(volume);
  sys_printf("Wanted size: %zu bytes, Volume size: %zu bytes\n", size,
             volume_size);
  test_assert(volume_size >= size);

  // Finalize the volume
  fs_vol_finalize(volume);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestMain("test_fs_01", test_fs_01); }
