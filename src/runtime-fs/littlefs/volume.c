#include "lfs.h"
#include "private.h"
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <string.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

/**
 * @brief Return total addressable size of a mounted volume.
 */
size_t fs_vol_size(fs_volume_t *volume, size_t *free) {
  if (volume == NULL) {
    return 0;
  }
  if (free) {
    lfs_ssize_t used = lfs_fs_size(&volume->lfs);
    size_t total_blocks = volume->storage_size / volume->cfg.block_size;
    *free = (used < 0 || (size_t)used > total_blocks)
                ? 0
                : (total_blocks - (size_t)used) * volume->cfg.block_size;
  }
  return volume->storage_size;
}

void fs_vol_finalize(fs_volume_t *volume) {
  if (volume == NULL) {
    return;
  }

  // Unmount
  lfs_unmount(&volume->lfs);

  // Close underlying file if file-backed
  if (volume->file) {
    FILE *fp = (FILE *)volume->storage;
    if (fp) {
      fflush(fp);
      int fd = fileno(fp);
      if (fd >= 0)
        fsync(fd);
      fclose(fp);
    }
  }

  // Free storage
  volume->storage = NULL;
  sys_free(volume);
}

/**
 * @brief Retrieve information about a specific file or directory.
 */
fs_file_t fs_vol_stat(fs_volume_t *volume, const char *path) {
  sys_assert(volume);

  // Initialize empty result
  fs_file_t result = {0};
  if (!path || *path == '\0') {
    path = "/"; // default to root
  }

  struct lfs_info info;
  int r = lfs_stat(&volume->lfs, path, &info);
  if (r < 0) {
    // Not found or error; return zeroed struct
    return result;
  }

  // Populate result
  result.volume = volume;
  sys_memcpy(result.path, path, FS_PATH_MAX);
  result.size = (info.type == LFS_TYPE_REG) ? info.size : 0;
  result.dir = (info.type == LFS_TYPE_DIR);

  // Derive name pointer: last component after '/'
  const char *last = strrchr(path, '/');
  result.name = (last && *(last + 1)) ? last + 1 : path; // skip leading '/'

  // Return populated struct
  return result;
}

/**
 * @brief Remove a file or (empty) directory.
 */
bool fs_vol_delete(fs_volume_t *volume, const char *path) {
  sys_assert(volume);
  if (path == NULL || *path == '\0' || sys_strcmp(path, "/") == 0) {
    return false; // cannot delete root
  }
  return lfs_remove(&volume->lfs, path) == 0;
}
