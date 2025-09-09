#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <string.h>
#include <tests/tests.h>

static void assert_file(fs_volume_t *vol, const char *path, const void *data,
                        size_t len) {
  fs_file_t st = fs_vol_stat(vol, path);
  test_assert(!st.dir && st.size == len);
  fs_file_t f = fs_file_open(vol, path, false);
  test_assert(f.ctx);
  test_assert(fs_file_seek(&f, 0));
  uint8_t buf[512];
  size_t n = fs_file_read(&f, buf, len);
  test_assert(n == len);
  test_assert(memcmp(buf, data, len) == 0);
  test_assert(fs_file_close(&f));
}

int test_fs_09(void) {
  fs_volume_t *vol = fs_vol_init_memory(64 * 1024);
  test_assert(vol);

  // Create directories
  test_assert(fs_vol_mkdir(vol, "/a"));
  test_assert(fs_vol_mkdir(vol, "/b"));

  // Create file /a/foo.txt with content
  const char *p1 = "/a/foo.txt";
  const char *p2 = "/a/bar.txt";
  const char *p3 = "/b/baz.txt";
  const char data1[] = "alpha";
  const char data2[] = "beta";

  fs_file_t f = fs_file_create(vol, p1);
  test_assert(f.ctx);
  size_t n = fs_file_write(&f, data1, strlen(data1));
  test_assert(n == strlen(data1));
  test_assert(fs_file_close(&f));

  assert_file(vol, p1, data1, strlen(data1));

  // Rename /a/foo.txt -> /a/bar.txt
  test_assert(fs_vol_move(vol, p1, p2));
  // Old path should be gone
  fs_file_t st_old = fs_vol_stat(vol, p1);
  test_assert(st_old.name[0] == '\0');
  // New path should exist with same content
  assert_file(vol, p2, data1, strlen(data1));

  // Move /a/bar.txt -> /b/baz.txt
  test_assert(fs_vol_move(vol, p2, p3));
  fs_file_t st_mid = fs_vol_stat(vol, p2);
  test_assert(st_mid.name[0] == '\0');
  assert_file(vol, p3, data1, strlen(data1));

  // Create nested dir and move directory
  test_assert(fs_vol_mkdir(vol, "/c"));
  const char *pdir = "/c/dir";
  test_assert(fs_vol_mkdir(vol, pdir));
  const char *pnested = "/c/dir/n.txt";
  fs_file_t g = fs_file_create(vol, pnested);
  test_assert(g.ctx);
  n = fs_file_write(&g, data2, strlen(data2));
  test_assert(n == strlen(data2));
  test_assert(fs_file_close(&g));
  assert_file(vol, pnested, data2, strlen(data2));

  // Rename directory /c/dir -> /c/dir2
  const char *pdir2 = "/c/dir2";
  test_assert(fs_vol_move(vol, pdir, pdir2));
  const char *pnested2 = "/c/dir2/n.txt";
  assert_file(vol, pnested2, data2, strlen(data2));

  fs_vol_finalize(vol);
  return 0;
}

int main(void) { return TestMain("test_fs_09", test_fs_09); }
