#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <string.h>
#include <tests/tests.h>

static const char *kFilePath = "/big.bin";

static void fill_pattern(uint8_t *buf, size_t n, uint8_t seed) {
  for (size_t i = 0; i < n; ++i)
    buf[i] = (uint8_t)(seed + i);
}

int test_fs_07(void) {
  // Big-ish RAM volume to cross block boundaries
  fs_volume_t *vol = fs_vol_init_memory(128 * 1024);
  test_assert(vol);

  // 1) Large write crossing blocks
  fs_file_t f = fs_file_create(vol, kFilePath);
  test_assert(f.ctx);
  const size_t N = 8192 + 123; // likely crosses multiple LFS blocks
  static uint8_t wbuf[9000];
  static uint8_t rbuf[9000];
  fill_pattern(wbuf, N, 0x10);
  size_t n = fs_file_write(&f, wbuf, N);
  test_assert(n == N);
  test_assert(fs_file_seek(&f, 0));
  n = fs_file_read(&f, rbuf, N);
  test_assert(n == N);
  test_assert(memcmp(wbuf, rbuf, N) == 0);

  // 2) Overwrite in the middle (unaligned)
  const size_t off = 4097; // intentionally not aligned
  const uint8_t patch[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
  test_assert(fs_file_seek(&f, off));
  n = fs_file_write(&f, patch, sizeof(patch));
  test_assert(n == sizeof(patch));

  // Verify overwrite took and size unchanged
  test_assert(fs_file_seek(&f, 0));
  n = fs_file_read(&f, rbuf, N);
  test_assert(n == N);
  test_assert(memcmp(rbuf + off, patch, sizeof(patch)) == 0);
  test_assert(f.size == N);

  // 3) Invalid large seek should fail (overflow guard)
  test_assert(!fs_file_seek(&f, (size_t)-1));

  // 4) Seek beyond EOF + write tail should extend file
  const char tail[] = "XYZ";
  const size_t new_off = N + 100; // gap
  test_assert(fs_file_seek(&f, new_off));
  n = fs_file_write(&f, tail, strlen(tail));
  test_assert(n == strlen(tail));
  size_t new_size = f.size;
  test_assert(new_size == new_off + strlen(tail));

  test_assert(fs_file_close(&f));

  // Reopen read-only and validate tail content
  fs_file_t rd = fs_file_open(vol, kFilePath, false);
  test_assert(rd.ctx);
  test_assert(rd.size == new_size);
  test_assert(fs_file_seek(&rd, new_off));
  char tailbuf[4] = {0};
  n = fs_file_read(&rd, tailbuf, strlen(tail));
  test_assert(n == strlen(tail));
  test_assert(memcmp(tailbuf, tail, strlen(tail)) == 0);
  test_assert(fs_file_close(&rd));

  fs_vol_finalize(vol);
  return 0;
}

int main(void) { return TestMain("test_fs_07", test_fs_07); }
