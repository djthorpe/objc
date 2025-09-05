#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_fs_02(void) {
  size_t size = 1024;

  // Create a memory-backed file system volume
  fs_volume_t *volume = fs_vol_init_memory(size);
  test_assert(volume);

  // Read the root directory (should be empty)
  sys_printf("Root directory entries:\n");
  fs_file_t file;
  while (fs_vol_readdir(volume, "/", &file)) {
    sys_printf(" - %s\n", file.name);
  }

  // Finalize the volume
  fs_vol_finalize(volume);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestMain("test_fs_02", test_fs_02); }
