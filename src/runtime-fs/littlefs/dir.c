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
    iterator->name = NULL;
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

  // Build full path: normalize base directory path then append entry name.
  char *dst = iterator->path;
  const char *src = path;
  size_t remaining = FS_PATH_MAX; // space left excluding final NUL

  // Copy full path
  while (remaining && *src) {
    *dst++ = *src++;
    remaining--;
  }

  // If nothing copied or first char wasn't '/', enforce root '/'
  if (iterator->path[0] != FS_PATH_SEPARATOR) {
    dst = iterator->path; // reset
    if (remaining < FS_PATH_MAX) {
      remaining = FS_PATH_MAX; // treat as fresh since we discard previous copy
    }
    *dst++ = FS_PATH_SEPARATOR;
    remaining--;
  }

  // Remove trailing slash if not root (dst currently at end)
  if ((dst - iterator->path) > 1 && *(dst - 1) == FS_PATH_SEPARATOR) {
    dst--; // drop it
    remaining++;
  }

  // Append separator if not root to prepare for entry name
  bool is_root =
      ((dst - iterator->path) == 1 && iterator->path[0] == FS_PATH_SEPARATOR);
  if (!is_root) {
    if (remaining) {
      *dst++ = FS_PATH_SEPARATOR;
      remaining--;
    }
  }

  // Set name pointer
  iterator->name = dst;

  // Append entry name
  const char *ename = info.name;
  while (remaining && *ename) {
    *dst++ = *ename++;
    remaining--;
  }

  // If we truncated the name (remaining==0 but ename not end) back up one for
  // NUL
  if (remaining == 0) {
    // Ensure space for NUL
    dst = iterator->path + FS_PATH_MAX; // last position
  }

  // NUL terminate
  *dst = '\0';

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
