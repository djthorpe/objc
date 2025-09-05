#include "lfs.h"
#include "private.h"
#include <runtime-fs/fs.h>
#include <runtime-sys/sys.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// BLOCK DEVICE CALLBACKS

static int _lfsbd_mem_read(const struct lfs_config *c, lfs_block_t block,
                           lfs_off_t off, void *buffer, lfs_size_t size) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  size_t addr = (size_t)block * c->block_size + off;
  if (!vol || addr + size > vol->storage_size) {
    return LFS_ERR_IO;
  }
  sys_memcpy(buffer, (uint8_t *)vol->storage + addr, size);
  return LFS_ERR_OK;
}

static int _lfsbd_mem_prog(const struct lfs_config *c, lfs_block_t block,
                           lfs_off_t off, const void *buffer, lfs_size_t size) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  size_t addr = (size_t)block * c->block_size + off;
  if (!vol || addr + size > vol->storage_size) {
    return LFS_ERR_IO;
  }
  sys_memcpy((uint8_t *)vol->storage + addr, buffer, size);
  return LFS_ERR_OK;
}

static int _lfsbd_mem_erase(const struct lfs_config *c, lfs_block_t block) {
  fs_volume_t *vol = (fs_volume_t *)c->context;
  if (!vol) {
    return LFS_ERR_IO;
  }
  size_t addr = (size_t)block * c->block_size;
  if (addr + c->block_size > vol->storage_size) {
    return LFS_ERR_IO;
  }
  sys_memset((uint8_t *)vol->storage + addr, 0xFF, c->block_size);
  return LFS_ERR_OK;
}

static int _lfsbd_mem_sync(const struct lfs_config *c) {
  (void)c; // Nothing to do for in-memory device
  return LFS_ERR_OK;
}

///////////////////////////////////////////////////////////////////////////////
// PUBLIC API

fs_volume_t *fs_vol_init_memory(size_t size) {
  if (size < (LFS_BLOCK_SIZE << 1)) {
    size = (LFS_BLOCK_SIZE << 1); // Minimum two blocks
  }

  // Single contiguous allocation
  size_t blocks = (size + LFS_BLOCK_SIZE - 1) / LFS_BLOCK_SIZE;
  size_t storage_size = blocks * LFS_BLOCK_SIZE;
  size_t alloc_size = sizeof(fs_volume_t) + 8 + storage_size;
  uint8_t *base = (uint8_t *)sys_malloc(alloc_size);
  if (base == NULL) {
    return NULL;
  }
  sys_memset(base, 0, sizeof(fs_volume_t));

  // Align storage to 8-byte boundary after struct
  fs_volume_t *vol = (fs_volume_t *)base;
  uintptr_t p = (uintptr_t)(base + sizeof(fs_volume_t));
  p = (p + 7u) & ~(uintptr_t)7u;
  vol->storage = (void *)p;
  vol->storage_size = storage_size;

  // Initialize config
  struct lfs_config *cfg = &vol->cfg;
  cfg->context = vol;
  cfg->read = _lfsbd_mem_read;
  cfg->prog = _lfsbd_mem_prog;
  cfg->erase = _lfsbd_mem_erase;
  cfg->sync = _lfsbd_mem_sync;
  cfg->read_size = LFS_READ_SIZE;
  cfg->prog_size = LFS_PROG_SIZE;
  cfg->block_size = LFS_BLOCK_SIZE;
  cfg->block_count = blocks;
  cfg->cache_size = LFS_CACHE_SIZE;
  cfg->lookahead_size = LFS_LOOKAHEAD_SIZE;
  cfg->block_cycles = 32;

  // Format (fresh volatile store)
  int r = lfs_format(&vol->lfs, &vol->cfg);
  if (r != LFS_ERR_OK) {
    sys_free(base);
    return NULL;
  }

  // Mount
  r = lfs_mount(&vol->lfs, &vol->cfg);
  if (r != LFS_ERR_OK) {
    sys_free(base);
    return NULL;
  }

  // Return the mounted volume
  return vol;
}
