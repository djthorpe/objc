#pragma once
#include "lfs.h"
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

// Basic parameters for the in-memory block device
#define LFS_BLOCK_SIZE 512u    // Erase/program block size
#define LFS_READ_SIZE 16u      // Minimum read size
#define LFS_PROG_SIZE 16u      // Minimum program size
#define LFS_CACHE_SIZE 64u     // Cache (must be >= read/prog size)
#define LFS_LOOKAHEAD_SIZE 16u // Lookahead buffer size

struct fs_volume_t {
  lfs_t lfs;             // LittleFS instance
  struct lfs_config cfg; // Configuration
  void *storage;         // In-memory buffer OR FILE* when file-backed
  size_t storage_size;   // Size of allocated storage/file image
  bool file;             // true if file-backed volume
};
