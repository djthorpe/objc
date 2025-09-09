#include "lfs.h"
#include "private.h"
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

/**
 * @brief Copy basename of path into dst buffer (NUL-terminated). Truncates if
 * needed.
 */
static void _fs_file_basename(char *dst, size_t dstsz, const char *path) {
  sys_assert(dst);
  sys_assert(path && path[0] != '\0');
  sys_assert(dstsz > 0);

  // Trim trailing separators
  const char *start = path;
  const char *end = path;
  while (*end) {
    end++;
  }
  while (end > start + 1 && *(end - 1) == FS_PATH_SEPARATOR) {
    end--;
  }

  // Find last separator before end by scanning backward
  const char *scan = end;
  while (scan > start && *(scan - 1) != FS_PATH_SEPARATOR) {
    scan--;
  }

  // Copy bounded and NUL-terminate
  const char *base = scan;
  size_t len = (size_t)(end - base);
  size_t n = (len < (dstsz - 1)) ? len : (dstsz - 1);
  if (n > 0) {
    sys_memcpy(dst, base, n);
  }
  dst[n] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

/**
 * @brief Create a new file or truncate an existing file to zero length.
 */
fs_file_t fs_file_create(fs_volume_t *volume, const char *path) {
  sys_assert(volume);
  sys_assert(path);

  // Disallow creating a file at the root path
  if (path[0] == '/' && path[1] == '\0') {
    return (fs_file_t){0};
  }

  // Create a file handle
  lfs_file_t *file = sys_malloc(sizeof(lfs_file_t));
  if (file == NULL) {
    return (fs_file_t){0};
  } else {
    sys_memset(file, 0, sizeof(lfs_file_t));
  }

  // Create or truncate file
  int r = lfs_file_open(&volume->lfs, file, path,
                        LFS_O_CREAT | LFS_O_TRUNC | LFS_O_RDWR);
  if (r < 0) {
    sys_free(file);
    return (fs_file_t){0};
  }

  // Populate result
  fs_file_t result = {0};
  result.volume = volume;
  _fs_file_basename(result.name, sizeof(result.name), path);
  result.size = 0;
  result.dir = false;
  result.pos = 0;
  result.ctx = (void *)file;

  // Return success
  return result;
}

/**
 * @brief Open an existing file for read/write.
 */
fs_file_t fs_file_open(fs_volume_t *volume, const char *path, bool write) {
  sys_assert(volume);
  sys_assert(path);

  // Disallow opening the root path as a file
  if (path[0] == '/' && path[1] == '\0') {
    return (fs_file_t){0};
  }

  // Create a file handle
  lfs_file_t *file = sys_malloc(sizeof(lfs_file_t));
  if (file == NULL) {
    return (fs_file_t){0};
  } else {
    sys_memset(file, 0, sizeof(lfs_file_t));
  }

  // Open file
  int flags = write ? LFS_O_RDWR : LFS_O_RDONLY;
  int r = lfs_file_open(&volume->lfs, file, path, flags);
  if (r < 0) {
    sys_free(file);
    return (fs_file_t){0};
  }

  // Populate result
  fs_file_t result = {0};
  result.volume = volume;
  _fs_file_basename(result.name, sizeof(result.name), path);
  result.dir = false;
  result.pos = 0;
  result.ctx = (void *)file;

  // Get file size
  lfs_soff_t sz = lfs_file_size(&volume->lfs, file);
  result.size = (sz < 0) ? 0u : (size_t)sz;

  // Return success
  return result;
}

/**
 * @brief Close an opened file.
 */
bool fs_file_close(fs_file_t *file) {
  sys_assert(file);
  sys_assert(file->volume);
  if (file->ctx == NULL) {
    return false; // not open
  }
  lfs_file_t *lfs_file = (lfs_file_t *)file->ctx;

  // Close the file
  int r = lfs_file_close(&file->volume->lfs, lfs_file);
  sys_free(lfs_file);
  file->ctx = NULL;
  return (r == 0);
}

/**
 * @brief Seek to a position within an opened file.
 */
bool fs_file_seek(fs_file_t *file, size_t offset) {
  sys_assert(file);
  sys_assert(file->volume);
  if (file->ctx == NULL) {
    return false; // not open
  }
  lfs_file_t *lfs_file = (lfs_file_t *)file->ctx;

  // Seek to the specified offset
  lfs_soff_t off = (lfs_soff_t)offset;
  if ((size_t)off != offset) {
    return false; // overflow converting to lfs_soff_t
  }
  lfs_soff_t pos =
      lfs_file_seek(&file->volume->lfs, lfs_file, off, LFS_SEEK_SET);
  if (pos < 0) {
    return false; // seek failed
  }

  // Update current position
  file->pos = (size_t)pos;
  return true;
}

/**
 * @brief Read data from an opened file.
 */
size_t fs_file_read(fs_file_t *file, void *buffer, size_t size) {
  sys_assert(file);
  sys_assert(file->volume);
  if (file->ctx == NULL) {
    return false; // not open
  }
  lfs_file_t *lfs_file = (lfs_file_t *)file->ctx;

  // Read data from the file
  size_t bytes_read = lfs_file_read(&file->volume->lfs, lfs_file, buffer, size);
  if (bytes_read == 0) {
    return 0; // read failed
  }

  // Update current position
  file->pos = lfs_file_tell(&file->volume->lfs, lfs_file);

  // Return bytes read
  return bytes_read;
}

/**
 * @brief Write data to an opened file.
 */
size_t fs_file_write(fs_file_t *file, const void *buffer, size_t size) {
  sys_assert(file);
  sys_assert(file->volume);
  if (file->ctx == NULL) {
    return false; // not open
  }
  lfs_file_t *lfs_file = (lfs_file_t *)file->ctx;
  if (size == 0) {
    return 0; // nothing to do
  }

  // Write data to the file
  size_t bytes_written =
      lfs_file_write(&file->volume->lfs, lfs_file, buffer, size);
  if (bytes_written == 0) {
    return 0; // write failed
  }

  // Update current position
  lfs_soff_t pos = lfs_file_tell(&file->volume->lfs, lfs_file);
  if (pos < 0) {
    file->pos += bytes_written;
  } else {
    file->pos = (size_t)pos;
  }

  // Update file size
  lfs_soff_t sz = lfs_file_size(&file->volume->lfs, lfs_file);
  if (sz < 0) {
    if (file->pos > file->size) {
      file->size = file->pos; // best effort
    } else {
      // leave unchanged
    }
  } else {
    file->size = (size_t)sz;
  }

  // Return bytes written
  return bytes_written;
}
