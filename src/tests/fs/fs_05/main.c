#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <tests/tests.h>

static const char *kFilePath = "/hello.txt";
static const char payload[] = "The quick brown fox jumps over the lazy dog";

int test_fs_05(void) {
  // Use in-memory volume
  fs_volume_t *vol = fs_vol_init_memory(16 * 1024);
  test_assert(vol);

  // Create file
  fs_file_t f = fs_file_create(vol, kFilePath);
  test_assert(f.ctx != NULL);

  // Write payload
  size_t wrote = fs_file_write(&f, payload, sizeof(payload));
  test_assert(wrote == sizeof(payload));

  // Seek to start
  bool ok = fs_file_seek(&f, 0);
  test_assert(ok);

  // Read back
  char buf[sizeof(payload)];
  size_t rd = fs_file_read(&f, buf, sizeof(buf));
  test_assert(rd == sizeof(payload));
  test_assert(sys_memcmp(buf, payload, sizeof(payload)) == 0);

  // Close
  ok = fs_file_close(&f);
  test_assert(ok);

  // Stat and check size
  fs_file_t st = fs_vol_stat(vol, kFilePath);
  test_assert(!st.dir);
  test_assert(st.size == sizeof(payload));

  // Cleanup
  fs_vol_finalize(vol);
  return 0;
}

int main(void) { return TestMain("test_fs_05", test_fs_05); }
