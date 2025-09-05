// LittleFS file-backed volume implementation
#include "lfs.h"
#include "private.h"
#include <errno.h>
#include <fcntl.h>
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////
// BLOCK DEVICE CALLBACKS

// Calculate absolute byte offset in underlying file
static inline off_t _fb_offset(const struct lfs_config *c, lfs_block_t block,
                               lfs_off_t off) {
  return (off_t)block * (off_t)c->block_size + (off_t)off;
}

static int _lfsbd_file_read(const struct lfs_config *c, lfs_block_t block,
                            lfs_off_t off, void *buffer, lfs_size_t size) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  sys_assert(vol);
  sys_assert(vol->file);

  FILE *fp = (FILE *)vol->storage;
  off_t pos = _fb_offset(c, block, off);
  if (fseeko(fp, pos, SEEK_SET) != 0)
    return LFS_ERR_IO;
  size_t rd = fread(buffer, 1, size, fp);
  if (rd != size)
    return LFS_ERR_IO;
  return LFS_ERR_OK;
}

static int _lfsbd_file_prog(const struct lfs_config *c, lfs_block_t block,
                            lfs_off_t off, const void *buffer,
                            lfs_size_t size) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  sys_assert(vol);
  sys_assert(vol->file);

  FILE *fp = (FILE *)vol->storage;
  off_t pos = _fb_offset(c, block, off);
  if (fseeko(fp, pos, SEEK_SET) != 0)
    return LFS_ERR_IO;
  size_t wr = fwrite(buffer, 1, size, fp);
  if (wr != size)
    return LFS_ERR_IO;
  return LFS_ERR_OK;
}

static int _lfsbd_file_erase(const struct lfs_config *c, lfs_block_t block) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  sys_assert(vol);
  sys_assert(vol->file);

  FILE *fp = (FILE *)vol->storage;
  off_t pos = _fb_offset(c, block, 0);
  if (fseeko(fp, pos, SEEK_SET) != 0)
    return LFS_ERR_IO;
  uint8_t buf[64];
  memset(buf, 0xFF, sizeof(buf));
  size_t remaining = c->block_size;
  while (remaining) {
    size_t chunk = remaining < sizeof(buf) ? remaining : sizeof(buf);
    size_t wr = fwrite(buf, 1, chunk, fp);
    if (wr != chunk)
      return LFS_ERR_IO;
    remaining -= chunk;
  }
  return LFS_ERR_OK;
}

static int _lfsbd_file_sync(const struct lfs_config *c) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  sys_assert(vol);
  sys_assert(vol->file);

  FILE *fp = (FILE *)vol->storage;
  if (fflush(fp) != 0)
    return LFS_ERR_IO;
  int fd = fileno(fp);
  if (fd >= 0 && fsync(fd) != 0)
    return LFS_ERR_IO;
  return LFS_ERR_OK;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

/**
 * @brief Initializes a filesystem-based (file-backed) volatile volume.
 */
fs_volume_t *fs_vol_init_file(const char *path, size_t size_bytes) {
  sys_assert(path);
  sys_assert(size_bytes > 0);

  // Establish target size: existing file size (if any) or requested size
  struct stat st;
  bool exists = (stat(path, &st) == 0);
  if (exists) {
    if (st.st_size > 0) {
      size_bytes = (size_t)st.st_size;
    }
  }
  if (size_bytes < (LFS_BLOCK_SIZE << 1)) {
    size_bytes = (LFS_BLOCK_SIZE << 1);
  }

  // Open or create file; keep FILE* open for lifetime
  size_t blocks = (size_bytes + LFS_BLOCK_SIZE - 1) / LFS_BLOCK_SIZE;
  size_t storage_size = blocks * LFS_BLOCK_SIZE;
  FILE *fp = fopen(path, exists ? "r+b" : "w+b");
  if (fp == NULL) {
    return NULL;
  }

  int fd = fileno(fp);
  if (fd < 0) {
    fclose(fp);
    return NULL;
  }

  // Determine current file size and extend if needed
  off_t current = lseek(fd, 0, SEEK_END);
  if (current < 0) {
    fclose(fp);
    return NULL;
  }
  if ((size_t)current < storage_size) {
    if (ftruncate(fd, (off_t)storage_size) != 0) {
      fclose(fp);
      return NULL;
    }
    if (lseek(fd, current, SEEK_SET) >= 0) {
      uint8_t buf[256];
      sys_memset(buf, 0xFF, sizeof(buf));
      size_t remaining = storage_size - (size_t)current;
      while (remaining) {
        size_t chunk = remaining < sizeof(buf) ? remaining : sizeof(buf);
        if (write(fd, buf, chunk) != (ssize_t)chunk) {
          fclose(fp);
          return NULL;
        }
        remaining -= chunk;
      }
    }
  }
  if (exists == false) {
    if (lseek(fd, 0, SEEK_SET) >= 0) {
      uint8_t buf[256];
      sys_memset(buf, 0xFF, sizeof(buf));
      size_t remaining = storage_size;
      while (remaining) {
        size_t chunk = remaining < sizeof(buf) ? remaining : sizeof(buf);
        if (write(fd, buf, chunk) != (ssize_t)chunk) {
          fclose(fp);
          return NULL;
        }
        remaining -= chunk;
      }
    }
  }
  lseek(fd, 0, SEEK_SET);

  // Allocate volume structure only (file acts as storage)
  fs_volume_t *vol = (fs_volume_t *)sys_malloc(sizeof(fs_volume_t));
  if (vol == NULL) {
    fclose(fp);
    return NULL;
  }

  // Set up volume structure
  sys_memset(vol, 0, sizeof(*vol));
  vol->storage = (void *)fp;
  vol->storage_size = storage_size;
  vol->file = true;

  // Set up LittleFS configuration
  struct lfs_config *cfg = &vol->cfg;
  cfg->context = vol;
  cfg->read = _lfsbd_file_read;
  cfg->prog = _lfsbd_file_prog;
  cfg->erase = _lfsbd_file_erase;
  cfg->sync = _lfsbd_file_sync;
  cfg->read_size = LFS_READ_SIZE;
  cfg->prog_size = LFS_PROG_SIZE;
  cfg->block_size = LFS_BLOCK_SIZE;
  cfg->block_count = blocks;
  cfg->cache_size = LFS_CACHE_SIZE;
  cfg->lookahead_size = LFS_LOOKAHEAD_SIZE;
  cfg->block_cycles = 32;

  // Try mounting existing content; if fails, format then mount
  int r = lfs_mount(&vol->lfs, &vol->cfg);
  if (r != LFS_ERR_OK) {
    if (lfs_format(&vol->lfs, &vol->cfg) != LFS_ERR_OK ||
        lfs_mount(&vol->lfs, &vol->cfg) != LFS_ERR_OK) {
      fclose(fp);
      sys_free(vol);
      return NULL;
    }
  }

  // Return success
  return vol;
}
