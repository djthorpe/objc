#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <string.h>
#include <tests/tests.h>

// Exercise partial writes across likely block boundaries with interleaved
// reads. We assume littlefs default block size is >= 256; choose offsets around
// 4096.

static const char *kFilePath = "/boundary.bin";

static void fill(uint8_t *p, size_t n, uint8_t base) {
  for (size_t i = 0; i < n; ++i)
    p[i] = (uint8_t)(base + i);
}

int test_fs_08(void) {
  fs_volume_t *vol = fs_vol_init_memory(128 * 1024);
  test_assert(vol);

  // Prepare buffers
  enum { A = 0x40, B = 0x60, C = 0x80 };
  const size_t base = 4096;         // near a typical block boundary
  const size_t w1_len = 300;        // spans boundary if starting near end
  const size_t w2_off = base - 50;  // straddles boundary backwards
  const size_t w2_len = 200;        // overlaps with w1
  const size_t w3_off = base + 100; // after boundary
  const size_t w3_len = 256;        // exact power-of-two length

  uint8_t w1[w1_len];
  fill(w1, w1_len, A);
  uint8_t w2[w2_len];
  fill(w2, w2_len, B);
  uint8_t w3[w3_len];
  fill(w3, w3_len, C);

  // Create file and perform interleaved writes/reads
  fs_file_t f = fs_file_create(vol, kFilePath);
  test_assert(f.ctx);

  // First write around boundary: start so that it crosses base
  size_t w1_off = base - 100; // 100 before boundary -> 200 after
  test_assert(fs_file_seek(&f, w1_off));
  size_t n = fs_file_write(&f, w1, w1_len);
  test_assert(n == w1_len);

  // Interleaved read: read across the boundary region covering start..end of w1
  test_assert(fs_file_seek(&f, base - 120));
  uint8_t rb[512];
  memset(rb, 0, sizeof(rb));
  n = fs_file_read(&f, rb, 240);
  test_assert(n == 240);
  // The last 220 bytes of this read should match the first 220 of w1
  test_assert(memcmp(rb + 20, w1, 220) == 0);

  // Second write overlapping part of w1
  test_assert(fs_file_seek(&f, w2_off));
  n = fs_file_write(&f, w2, w2_len);
  test_assert(n == w2_len);

  // Read a window that covers the overlap to ensure new data replaced old
  const size_t win_off = base - 80;
  const size_t win_len = 220;
  test_assert(fs_file_seek(&f, win_off));
  memset(rb, 0, sizeof(rb));
  n = fs_file_read(&f, rb, win_len);
  test_assert(n == win_len);
  // Expect first 30 bytes from B (overlap), then A resumes until boundary
  // Compute overlap extents
  size_t overlap_start = (w2_off > win_off) ? (w2_off - win_off) : 0;
  size_t overlap_end = ((w2_off + w2_len) < (win_off + win_len))
                           ? (w2_off + w2_len - win_off)
                           : win_len;
  // Verify overlapped region equals corresponding slice of w2
  test_assert(memcmp(rb + overlap_start,
                     w2 + (win_off + overlap_start - w2_off),
                     overlap_end - overlap_start) == 0);

  // Third write after boundary, non-overlapping with previous
  test_assert(fs_file_seek(&f, w3_off));
  n = fs_file_write(&f, w3, w3_len);
  test_assert(n == w3_len);

  // Validate slices separately
  // 1) Region before w1 remains 0x00 (unwritten -> implied holes)
  test_assert(fs_file_seek(&f, w1_off - 10));
  memset(rb, 0xFF, sizeof(rb));
  n = fs_file_read(&f, rb, 10);
  test_assert(n == 10);
  // Contents for unwritten area may be 0x00 or preserved erased pattern (often
  // 0xFF). We'll only verify that it is not equal to A/B/C pattern length-wise
  // comparisons later.

  // 2) Region covering w2 (consider possible overlap with w3)
  test_assert(fs_file_seek(&f, w2_off));
  memset(rb, 0, sizeof(rb));
  n = fs_file_read(&f, rb, w2_len);
  test_assert(n == w2_len);
  // Compute overlap between [w2_off, w2_off+w2_len) and [w3_off, w3_off+w3_len)
  const size_t w2_end = w2_off + w2_len;
  const size_t w3_end = w3_off + w3_len;
  const size_t ov_start = (w3_off > w2_off) ? w3_off : w2_off;
  const size_t ov_end = (w3_end < w2_end) ? w3_end : w2_end;
  const size_t ov_len = (ov_end > ov_start) ? (ov_end - ov_start) : 0;
  const size_t pre_len = (ov_start > w2_off) ? (ov_start - w2_off) : 0;
  const size_t post_len =
      (w2_len > (pre_len + ov_len)) ? (w2_len - pre_len - ov_len) : 0;
  if (pre_len > 0) {
    test_assert(memcmp(rb, w2, pre_len) == 0);
  }
  if (ov_len > 0) {
    size_t w3_slice_off = ov_start - w3_off;
    test_assert(memcmp(rb + pre_len, w3 + w3_slice_off, ov_len) == 0);
  }
  if (post_len > 0) {
    size_t w2_tail_off = pre_len + ov_len;
    test_assert(memcmp(rb + pre_len + ov_len, w2 + w2_tail_off, post_len) == 0);
  }

  // 3) Region covering w3
  test_assert(fs_file_seek(&f, w3_off));
  memset(rb, 0, sizeof(rb));
  n = fs_file_read(&f, rb, w3_len);
  test_assert(n == w3_len);
  test_assert(memcmp(rb, w3, w3_len) == 0);

  // Size should extend to end of w3
  size_t expected_size = w3_off + w3_len;
  test_assert(f.size == expected_size);

  test_assert(fs_file_close(&f));
  fs_vol_finalize(vol);
  return 0;
}

int main(void) { return TestMain("test_fs_08", test_fs_08); }
