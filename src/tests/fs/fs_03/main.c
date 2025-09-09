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
  size_t size = 1024 * 10;
  const char *path = "/tmp/test_fs_03_volume.img";

  // Create a file-backed file system volume
  fs_volume_t *volume = fs_vol_init_file(path, size);
  test_assert(volume);

  // Create some directories
  int i = 0;
  char dirpath[FS_PATH_MAX + 1];
  for (i = 0; i < 5; i++) {
    snprintf(dirpath, sizeof(dirpath), "/dir%d", i);
    bool ok = fs_vol_mkdir(volume, dirpath);
    test_assert(ok);
  }

  // Read the root directory (should be empty)
  sys_printf("Root directory entries:\n");
  fs_file_t file = (fs_file_t){0};
  while (fs_vol_readdir(volume, "/", &file)) {
    sys_printf("name='%s' type=%-4s size=%zu\n", file.name,
               file.dir ? "DIR" : "FILE", file.size);
  }

  // Print total size and used size
  size_t free_bytes = 0;
  size_t total_bytes = fs_vol_size(volume, &free_bytes);
  sys_printf("Volume size: total=%zu bytes, free=%zu bytes, used=%zu bytes\n",
             total_bytes, free_bytes, total_bytes - free_bytes);

  // Finalize the volume
  fs_vol_finalize(volume);

  // Cleanup: remove image file and directory (ignore errors)
  unlink(path);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN

int main(void) { return TestMain("test_fs_03", test_fs_03); }
