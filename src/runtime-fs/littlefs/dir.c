#include "lfs.h"
#include "private.h"
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Iterator structure
struct fs_iter_t {
  lfs_dir_t dir;
  bool open;
};

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

/**
 * @brief Reads a directory and returns successive entries.
 */
bool fs_vol_readdir(fs_volume_t *volume, const char *path,
                    fs_file_t *iterator) {
  sys_assert(volume);
  sys_assert(iterator);

  // Set default path if NULL/empty
  if (path == NULL || *path == '\0') {
    // TODO: Use FS_PATH_SEPARATOR
    path = "/"; // default root
  }

  // We create some state to track iteration progress.
  if (iterator->ctx == NULL) {
    struct fs_iter_t *st =
        (struct fs_iter_t *)sys_malloc(sizeof(struct fs_iter_t));
    if (st == NULL) {
      return false; // allocation failure
    }

    sys_memset(st, 0, sizeof(struct fs_iter_t));
    iterator->ctx = st;
  }

  // Open the iterator state
  struct fs_iter_t *st = (struct fs_iter_t *)iterator->ctx;
  sys_assert(st);
  if (st->open == false) {
    if (lfs_dir_open(&volume->lfs, &st->dir, path) < 0) {
      st->open = false;
      iterator->ctx = NULL;
      sys_free(st);
      return false;
    } else {
      st->open = true;
    }
  }

  // Read next entry
  struct lfs_info info;
  int res = lfs_dir_read(&volume->lfs, &st->dir, &info);
  if (res < 0) {
    // Error - terminate iteration
    lfs_dir_close(&volume->lfs, &st->dir);
    iterator->ctx = NULL;
    sys_free(st);
    return false;
  }

  // End of directory
  if (res == 0) {
    lfs_dir_close(&volume->lfs, &st->dir);
    iterator->name[0] = '\0';
    iterator->dir = false;
    iterator->size = 0;
    iterator->ctx = NULL;
    sys_free(st);
    return false;
  }

  // Skip "." and ".." entries
  if (sys_strcmp(info.name, ".") == 0 || sys_strcmp(info.name, "..") == 0) {
    return fs_vol_readdir(volume, path, iterator);
  }

  // Populate the iterator result
  iterator->volume = volume;
  iterator->dir = (info.type == LFS_TYPE_DIR);
  iterator->size = (info.type == LFS_TYPE_REG) ? info.size : 0;
  sys_memcpy(iterator->name, info.name, sizeof(iterator->name));

  // Return success
  return true;
}

/**
 * @brief Create a directory.
 */
bool fs_vol_mkdir(fs_volume_t *volume, const char *path) {
  sys_assert(volume);
  if (path == NULL || *path == '\0' || sys_strcmp(path, "/") == 0) {
    return false; // invalid path - cannot create root directory
  }

  int r = lfs_mkdir(&volume->lfs, path);
  if (r < 0) {
    if (r == LFS_ERR_EXIST) {
      return true; // already exists is not an error
    }
    return false; // other error
  }

  return true;
}
