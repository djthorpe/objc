#include <limits.h>
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <stdlib.h>
#include <tests/tests.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// TEST

int test_fs_03(void) {
  size_t size = 1024;
  const char *path = "/tmp/test_fs_03_volume.img";

  // Create a file-backed file system volume
  fs_volume_t *volume = fs_vol_init_file(path, size);
  test_assert(volume);

  // Read the root directory (should be empty)
  sys_printf("Root directory entries:\n");
  fs_file_t file;
  while (fs_vol_readdir(volume, "/", &file)) {
    sys_printf(" - %s\n", file.name);
  }

  // Finalize the volume
  fs_vol_finalize(volume);

  // Cleanup: remove image file and directory (ignore errors)
  unlink(path);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestMain("test_fs_03", test_fs_03); }
