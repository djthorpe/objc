/**
 * @file fs.h
 * @brief File system abstraction layer
 * @defgroup FileSystem File System Interfaces
 * @ingroup System
 *
 * Managing file system operations, including reading and writing files.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

typedef struct fs_volume_t fs_volume_t;

typedef struct {
  fs_volume_t *volume; // Owning volume
  bool dir;            // True if directory, false if file
  const char *path;    // Full path within volume, or NULL if root directory
  const char *name;    // File name (only for files, not directories)
  size_t size;         // Size in bytes (for files)
} fs_file_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Initializes a volatile memory-backed file system volume.
 * @ingroup FileSystem
 *
 * Create a new in-memory file system volume with the specified size.
 * The actual size will be equal or greater than the requested size.
 * If the volume is already initialized, it will be finalized and replaced.
 */
extern fs_volume_t *fs_vol_init_memory(size_t size);

/**
 * @brief Initializes a filesystem-based volume.
 * @ingroup FileSystem
 *
 * Open an existing, or create a new filesystem-based volume with the specified
 * size. The actual size will be equal or greater than the requested size. If
 * the volume already exists, an attempt will be made to extend the filesystem
 * to the requested size.
 *
 * If files are not supported on the platform, this function will return NULL.
 */
extern fs_volume_t *fs_vol_init_file(const char *path, size_t size);

/**
 * @brief Cleans up the file system volume.
 * @ingroup FileSystem
 *
 * This function should be called to dismount and release resources
 * associated with the file system volume.
 */
extern void fs_vol_finalize(fs_volume_t *volume);

/**
 * @brief Determines if the file system volume is valid and usable.
 * @ingroup FileSystem
 *
 * This function checks if the provided volume handle is initialized
 * and ready for file operations. It returns the size of the volume
 * in bytes if valid, or zero if invalid.
 */
extern size_t fs_vol_size(fs_volume_t *volume);

/**
 * @brief Reads a directory and returns the next file entry.
 * @ingroup FileSystem
 *
 * This function iterates over the entries in the specified
 * directory path within the given volume. It will return false
 * when there are no more entries to read.
 */
extern bool fs_vol_readdir(fs_volume_t *volume, const char *path,
                           fs_file_t *iterator);

/**
 * @brief Retrieve information about a specific file or directory.
 * @ingroup FileSystem
 * This function returns a fs_file_t structure with details about
 * the specified path within the volume. If the path does not exist,
 * the returned structure will have name set to NULL.
 */
extern fs_file_t fs_vol_stat(fs_volume_t *volume, const char *path);
