/**
 * @file fs.h
 * @brief File system abstraction layer for lightweight embedded & host storage.
 *
 * @defgroup FileSystem File System Runtime
 * @ingroup System
 *
 * High–level API wrapping an underlying littlefs-based implementation that can
 * operate either purely in RAM or backed by a host file (persisted across
 * runs).
 *
 * Thread-safety: Functions are NOT thread-safe; the
 * caller must serialize access to a volume. Returned pointers (paths/names)
 * reference caller-managed or internal static memory and must not be freed.
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// GLOBALS

#define FS_PATH_MAX 255       ///< Maximum length of a single path component
#define FS_PATH_SEPARATOR '/' ///< Path separator character

///////////////////////////////////////////////////////////////////////////////
// TYPES

typedef struct fs_volume_t fs_volume_t; ///< Opaque filesystem volume handle

/**
 * @brief File and directory metadata.
 * @ingroup FileSystem
 */
typedef struct {
  fs_volume_t *volume;        ///< Owning volume (set by APIs)
  bool dir;                   ///< True if directory, false if regular file
  char path[FS_PATH_MAX + 1]; ///< Full path within volume (never NULL
                              ///< after success) including leading '/'
                              ///< and trailing '\0'
  const char *name; ///< Basename component within the directory, can be NULL if
                    ///< it's the root directory
  size_t size;      ///< Size in bytes (regular files only, else 0)
  void *ctx;        ///< Opaque iterator cursor (do not modify)
} fs_file_t;

///////////////////////////////////////////////////////////////////////////////
// LIFECYCLE

/**
 * @brief Create a new volatile (RAM) filesystem volume.
 * @ingroup FileSystem
 *
 * @param size Requested minimum size in bytes (rounded up to block geometry).
 * @return Pointer to mounted volume on success, NULL on failure.
 *
 * Notes:
 *  - Contents are lost when fs_vol_finalize() is called or the process exits.
 *  - The real capacity may be larger than requested due to block rounding.
 */
extern fs_volume_t *fs_vol_init_memory(size_t size);

/**
 * @brief Open or create a host file–backed persistent volume.
 * @ingroup FileSystem
 *
 * Attempts to mount the filesystem stored in @p path. If mounting fails, a
 * format is performed and a fresh filesystem created. If the file is shorter
 * than @p size it may be expanded (filling new space with erased pattern 0xFF).
 *
 * @param path Host path to the image file (created if absent).
 * @param size Minimum size in bytes; ignored if existing file is larger.
 * @return Mounted volume pointer, or NULL on error.
 */
extern fs_volume_t *fs_vol_init_file(const char *path, size_t size);

/**
 * @brief Unmount and release all resources for a volume.
 * @ingroup FileSystem
 *
 * @param volume Volume returned from an init call (may be NULL).
 *
 * Safe to call with NULL (no effect). After return the pointer is invalid.
 */
extern void fs_vol_finalize(fs_volume_t *volume);

/**
 * @brief Return total addressable size of a mounted volume.
 * @ingroup FileSystem
 *
 * @param volume Volume handle.
 * @param free Pointer to size_t to receive approximate free space in bytes (may
 * be NULL).
 * @return Size in bytes, or 0 if @p volume is NULL/invalid.
 */
extern size_t fs_vol_size(fs_volume_t *volume, size_t *free);

/**
 * @brief Iterate directory entries.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param path Directory path ("/" for root). NULL treated as root.
 * @param iterator In/out. Provide zeroed struct to start; do NOT alter ctx.
 * @return true if an entry was produced (fields populated), false when done.
 */
extern bool fs_vol_readdir(fs_volume_t *volume, const char *path,
                           fs_file_t *iterator);

/**
 * @brief Lookup file or directory metadata for a path.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param path Absolute path within volume (NULL/empty -> root).
 * @return Populated fs_file_t. If not found, name == NULL.
 */
extern fs_file_t fs_vol_stat(fs_volume_t *volume, const char *path);

/**
 * @brief Create a directory.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param path New directory path (parents must already exist).
 * @return true on success or if directory already exists; false on error.
 */
extern bool fs_vol_mkdir(fs_volume_t *volume, const char *path);

/**
 * @brief Remove a file or (empty) directory.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param path Path to remove.
 * @return true on success; false if not found or directory not empty.
 */
extern bool fs_vol_remove(fs_volume_t *volume, const char *path);

/**
 * @brief Move or rename a file/directory.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param old_path Existing path.
 * @param new_path Destination path (must not already exist).
 * @return true on success, false on error (missing source, conflict, etc.).
 */
extern bool fs_vol_move(fs_volume_t *volume, const char *old_path,
                        const char *new_path);

/**
 * @brief Create a new file or truncate an existing file to zero length.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param path Path to create.
 * @return Opened read/write file, or zeroed struct on failure.
 */
fs_file_t fs_file_create(fs_volume_t *volume, const char *path);

/**
 * @brief Open an existing file for read/write.
 * @ingroup FileSystem
 *
 * @param volume Mounted volume.
 * @param path File path to open.
 * @param write True to open for writing, false for read-only.
 * @return Opened file, or zeroed struct on failure.
 */
fs_file_t fs_file_open(fs_volume_t *volume, const char *path, bool write);

/**
 * @brief Close an opened file.
 * @ingroup FileSystem
 *
 * @param file File to close.
 */
bool fs_file_close(fs_file_t *file);

/**
 * @brief Read data from an opened file.
 * @ingroup FileSystem
 *
 * @param file File to read from.
 * @param buffer Buffer to read data into.
 * @param size Number of bytes to read (cannot be zero).
 * @return Number of bytes read, or 0 on error.
 */
size_t fs_file_read(fs_file_t *file, void *buffer, size_t size);

/**
 * @brief Write data from an opened file.
 * @ingroup FileSystem
 *
 * @param file File to write to.
 * @param buffer Buffer to write data from.
 * @param size Number of bytes to write (cannot be zero).
 * @return Number of bytes written, or 0 on error.
 */
size_t fs_file_write(fs_file_t *file, const void *buffer, size_t size);

/**
 * @brief Seek to a position within an opened file.
 * @ingroup FileSystem
 *
 * @param file File to seek.
 * @param offset Offset in bytes from the beginning of the file.
 * @return true on success, false on error.
 */
bool fs_file_seek(fs_file_t *file, size_t offset);
