/**
 * @file hash.h
 * @brief Defines the ability to create hashes from data.
 *
 * This file declares various system methods for hash generation using OpenSSL.
 * Supports MD5 and SHA-256 algorithms with a simple, consistent API.
 *
 * Usage example:
 * @code
 * sys_hash_t hash = sys_hash_init(sys_hash_sha256);
 * sys_hash_update(&hash, "hello", 5);
 * sys_hash_update(&hash, "world", 5);
 * const uint8_t *result = sys_hash_finalize(&hash);
 * @endcode
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hash algorithm identifiers.
 *
 * Enumeration of supported cryptographic hash algorithms.
 */
typedef enum {
  sys_hash_md5,    // MD5 hash (128-bit)
  sys_hash_sha256, // SHA-256 hash (256-bit)
} sys_hash_algorithm_t;

/**
 * @brief Hash context structure.
 *
 * Contains the state and result buffer for hash operations.
 */
typedef struct {
  uint8_t hash[32]; // Buffer to hold the hash value = max is SHA-256 (32 bytes)
  size_t size;      // Size of the hash in bytes
  void *ctx;        // Any context needed for the hash operation
} sys_hash_t;

/**
 * @brief Initializes a new hash context for the specified algorithm.
 * @param algorithm The hash algorithm to use.
 * @return A new sys_hash_t instance initialized for the specified algorithm.
 *
 * This function returns a sys_hash_t instance for the
 * specified hash algorithm, ready for use in hashing operations. You must
 * call sys_hash_final() to finalize the hash computation, even on failure
 * of sys_hash_update().
 *
 * @note If a specific algorithm is not supported, the context will be
 * initialized with size 0. You should check sys_hash_size() to verify if
 * the context is usable.
 */
extern sys_hash_t sys_hash_init(sys_hash_algorithm_t algorithm);

/**
 * @brief Returns the size of the hash in bytes.
 * @param hash The hash context to query.
 * @return The size of the hash in bytes.
 *
 * This function returns the size of the hash value stored
 * in the sys_hash_t instance, which is determined by the algorithm used.
 * If the hashing failed, the size will be 0.
 */
extern size_t sys_hash_size(sys_hash_t *hash);

/**
 * @brief Updates the hash context with new data.
 * @param hash The hash context to update.
 * @param data The data to add to the hash.
 * @param size The size of the data in bytes.
 * @return true on success, false on failure.
 */
extern bool sys_hash_update(sys_hash_t *hash, const void *data, size_t size);

/**
 * @brief Finalizes the hash computation and returns the hash value.
 * @param hash The hash context to finalize.
 * @return A pointer to the computed hash value, or NULL on failure.
 *
 * This function finalizes the hash computation and returns
 * a pointer to the computed hash value stored in the sys_hash_t instance.
 * The context is automatically cleaned up after finalization.
 */
extern const uint8_t *sys_hash_finalize(sys_hash_t *hash);

#ifdef __cplusplus
}
#endif
