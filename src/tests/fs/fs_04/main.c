#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tests/tests.h>

// Stress parameters
#define STRESS_DIR_COUNT 16
#define STRESS_DEPTH 3

static void make_path(char *buf, size_t buflen, const char *prefix, int depth,
                      int index) {
  // prefix + /lvl<d>_<i>
  size_t len = 0;
  if (prefix && *prefix) {
    if (buflen) {
      strncpy(buf, prefix, buflen - 1);
      buf[buflen - 1] = '\0';
      len = strlen(buf);
    }
  } else {
    if (buflen) {
      buf[0] = '/';
      buf[1] = '\0';
      len = 1;
    }
  }
  if (len > 0 && buf[len - 1] != '/' && len < buflen - 1) {
    buf[len++] = '/';
    buf[len] = '\0';
  }
  char tmp[32];
  snprintf(tmp, sizeof(tmp), "lvl%d_%d", depth, index);
  if (buflen > len) {
    strncpy(buf + len, tmp, buflen - len - 1);
    buf[buflen - 1] = '\0';
  }
}

static void create_tree(fs_volume_t *vol) {
  char path[FS_PATH_MAX + 1];
  for (int d = 0; d < STRESS_DEPTH; ++d) {
    for (int i = 0; i < STRESS_DIR_COUNT; i++) {
      // Build parent prefix (root for now; could nest)
      make_path(path, sizeof(path), "/", d, i);
      bool ok = fs_vol_mkdir(vol, path);
      test_assert(ok);
    }
  }
}

static size_t count_entries(fs_volume_t *vol, const char *dir) {
  fs_file_t it = (fs_file_t){0};
  size_t count = 0;
  while (fs_vol_readdir(vol, dir, &it)) {
    count++;
  }
  return count;
}

static void delete_tree(fs_volume_t *vol) {
  char path[FS_PATH_MAX + 1];
  for (int d = 0; d < STRESS_DEPTH; ++d) {
    for (int i = 0; i < STRESS_DIR_COUNT; i++) {
      make_path(path, sizeof(path), "/", d, i);
      bool ok = fs_vol_remove(vol, path);
      test_assert(ok);
    }
  }
}

int test_fs_04(void) {
  // Larger volume to reduce early exhaustion
  fs_volume_t *vol = fs_vol_init_memory(64 * 1024);
  test_assert(vol);

  size_t free_before = 0;
  size_t total = fs_vol_size(vol, &free_before);
  sys_printf("Before: total=%zu free=%zu\n", total, free_before);

  create_tree(vol);
  size_t after_create_free = 0;
  fs_vol_size(vol, &after_create_free);
  size_t root_entries = count_entries(vol, "/");
  sys_printf("After create: root entries=%zu free=%zu used=%zu\n", root_entries,
             after_create_free, total - after_create_free);

  // Stat random subset
  for (int i = 0; i < 10; i++) {
    char path[FS_PATH_MAX + 1];
    make_path(path, sizeof(path), "/", i % STRESS_DEPTH, i);
    fs_file_t st = fs_vol_stat(vol, path);
    test_assert(st.name != NULL && st.dir);
  }

  delete_tree(vol);
  size_t after_delete_free = 0;
  fs_vol_size(vol, &after_delete_free);
  size_t remaining = count_entries(vol, "/");
  sys_printf("After delete: remaining=%zu free=%zu used=%zu\n", remaining,
             after_delete_free, total - after_delete_free);
  test_assert(remaining == 0);

  fs_vol_finalize(vol);
  return 0;
}

int main(void) { return TestMain("test_fs_04", test_fs_04); }
