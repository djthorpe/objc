#include "lfs.h"
#include "private.h"
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

/**
 * @brief Reads a directory and returns successive entries.
 *
 * Usage:
 *   fs_file_t it = {0};
 *   while(fs_vol_readdir(vol, "/", &it)) { ...process it... }
 *   // When function returns false and it.name == NULL -> end of listing.
 */
bool fs_vol_readdir(fs_volume_t *volume, const char *path, fs_file_t *it) {
  if (!volume || !it) {
    return false;
  }
  if (!path) {
    path = "/"; // default root
  }

  // TODO
  // We stash a littlefs directory handle inside iterator via opaque storage.
  typedef struct {
    lfs_dir_t dir;
    bool open;
    char name[256];
    bool first; // first read after open
  } _lfs_iter_t;

  // We borrow memory from iterator->name pointer field to store our state.
  // TODO: NOT thread-safe
  static _lfs_iter_t state;
  _lfs_iter_t *st = &state;

  if (!st->open || st->first) {
    // (Re)open directory on first call or after reset.
    if (st->open) {
      lfs_dir_close(&volume->lfs, &st->dir);
      st->open = false;
    }
    if (lfs_dir_open(&volume->lfs, &st->dir, path) < 0) {
      st->open = false;
      return false;
    }
    st->open = true;
    st->first = false;
  }

  struct lfs_info info;
  int res = lfs_dir_read(&volume->lfs, &st->dir, &info);
  if (res < 0) {
    // Error - terminate iteration
    lfs_dir_close(&volume->lfs, &st->dir);
    st->open = false;
    it->name = NULL;
    it->path = NULL;
    it->dir = false;
    it->size = 0;
    return false;
  }
  if (res == 0) {
    // End of directory
    if (st->open) {
      lfs_dir_close(&volume->lfs, &st->dir);
      st->open = false;
    }
    it->name = NULL;
    it->path = NULL;
    it->dir = false;
    it->size = 0;
    return false;
  }

  // Skip "." and ".." entries
  if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0) {
    return fs_vol_readdir(volume, path, it);
  }

  // Populate iterator fields
  strncpy(st->name, info.name, sizeof(st->name) - 1);
  st->name[sizeof(st->name) - 1] = '\0';
  it->name = st->name;
  it->path = st->name; // For now path==name within provided directory root
  it->dir = (info.type == LFS_TYPE_DIR);
  it->size = (info.type == LFS_TYPE_REG) ? info.size : 0;

  return true;
}
