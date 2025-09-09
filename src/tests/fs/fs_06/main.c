#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <string.h>
#include <tests/tests.h>

static const char *kFilePath = "/append.txt";

static void assert_mem(const void *a, const void *b, size_t n) {
  test_assert(memcmp(a, b, n) == 0);
}

int test_fs_06(void) {
  fs_volume_t *vol = fs_vol_init_memory(32 * 1024);
  test_assert(vol);

  // Create and write partially in chunks
  fs_file_t f = fs_file_create(vol, kFilePath);
  test_assert(f.ctx);

  const char p1[] = "AAAA";      // 4
  const char p2[] = "BBBBBB";    // 6 (total 10)
  const char p3[] = "CCCCCCCCC"; // 9 (total 19)
  const size_t l1 = strlen(p1);
  const size_t l2 = strlen(p2);
  const size_t l3 = strlen(p3);

  size_t n;
  n = fs_file_write(&f, p1, l1);
  test_assert(n == l1);
  n = fs_file_write(&f, p2, l2);
  test_assert(n == l2);
  n = fs_file_write(&f, p3, l3);
  test_assert(n == l3);

  // Seek to start and read partially
  test_assert(fs_file_seek(&f, 0));
  char part1[8];
  char part2[8];
  memset(part1, 0, sizeof(part1));
  memset(part2, 0, sizeof(part2));
  n = fs_file_read(&f, part1, l1); // read first chunk
  test_assert(n == l1);
  assert_mem(part1, p1, l1);
  n = fs_file_read(&f, part2, l2); // read second chunk
  test_assert(n == l2);
  assert_mem(part2, p2, l2);

  // Read remaining bytes to EOF
  char rest[64];
  n = fs_file_read(&f, rest, sizeof(rest));
  test_assert(n == l3);

  // Next read at EOF should return 0
  size_t eof_n = fs_file_read(&f, rest, sizeof(rest));
  test_assert(eof_n == 0);

  test_assert(fs_file_close(&f));

  // Append by reopening write and seeking to end
  fs_file_t f2 = fs_file_open(vol, kFilePath, /*write=*/true);
  test_assert(f2.ctx);
  size_t original = f2.size;
  test_assert(fs_file_seek(&f2, original)); // seek end
  const char tail[] = "TAIL";
  const size_t l4 = strlen(tail);
  n = fs_file_write(&f2, tail, l4);
  test_assert(n == l4);
  test_assert(fs_file_close(&f2));

  // Verify final contents
  fs_file_t rd = fs_file_open(vol, kFilePath, /*write=*/false);
  test_assert(rd.ctx);
  size_t total = rd.size;
  test_assert(total == l1 + l2 + l3 + l4);
  test_assert(fs_file_seek(&rd, 0));
  char all[128];
  n = fs_file_read(&rd, all, total);
  test_assert(n == total);
  test_assert(fs_file_close(&rd));

  // Spot check tail at end
  assert_mem(all + total - l4, tail, l4);

  fs_vol_finalize(vol);
  return 0;
}

int main(void) { return TestMain("test_fs_06", test_fs_06); }
