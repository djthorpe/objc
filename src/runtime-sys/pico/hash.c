/**
 * @file hash.c
 * @brief Implements hash generation functionality using mbedTLS.
 *
 * This file implements various system methods for hash generation.
 */
#include <mbedtls/md5.h>
#include <mbedtls/sha256.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Initializes a new hash context for the specified algorithm.
 */
sys_hash_t sys_hash_init(sys_hash_algorithm_t algorithm) {
  sys_hash_t hash;
  hash.size = 0;
  hash.algorithm = 0;

  switch (algorithm) {
  case sys_hash_md5:
    if (sizeof(mbedtls_md5_context) > sizeof(hash.ctx.ctx)) {
      break;
    }
    mbedtls_md5_init((mbedtls_md5_context *)hash.ctx.ctx);
    // Use legacy starts function (mbedtls 2.x)
    if (mbedtls_md5_starts((mbedtls_md5_context *)hash.ctx.ctx) == 0) {
      hash.size = 16; // MD5 produces a 128-bit hash
      hash.algorithm = algorithm;
    }
    break;
  case sys_hash_sha256:
    if (sizeof(mbedtls_sha256_context) > sizeof(hash.ctx.ctx)) {
      break;
    }
    mbedtls_sha256_init((mbedtls_sha256_context *)hash.ctx.ctx);
    if (mbedtls_sha256_starts((mbedtls_sha256_context *)hash.ctx.ctx, 0) == 0) {
      hash.size = 32; // SHA-256 produces a 256-bit hash
      hash.algorithm = algorithm;
    }
    break;
  }

  // Return the initialized hash context
  return hash;
}

/**
 * @brief Returns the size of the hash in bytes.
 */
size_t sys_hash_size(sys_hash_t *hash) {
  if (hash) {
    return hash->size;
  }
  return 0; // Handle error case
}

/**
 * @brief Updates the hash context with new data.
 */
bool sys_hash_update(sys_hash_t *hash, const void *data, size_t size) {
  if (data == NULL || hash == NULL || hash->algorithm == 0 || hash->size == 0) {
    return false;
  }
  if (size == 0) {
    return true; // No data to update, consider it a success
  }
  switch (hash->algorithm) {
  case sys_hash_md5:
    return mbedtls_md5_update((mbedtls_md5_context *)hash->ctx.ctx, data,
                              size) == 0;
  case sys_hash_sha256:
    return mbedtls_sha256_update((mbedtls_sha256_context *)hash->ctx.ctx, data,
                                 size) == 0;
  }
  return false;
}

/**
 * @brief Finalizes the hash computation and returns the hash value.
 */
const uint8_t *sys_hash_finalize(sys_hash_t *hash) {
  if (hash == NULL || hash->algorithm == 0 || hash->size == 0) {
    return NULL; // Invalid context
  }

  bool result = false;
  switch (hash->algorithm) {
  case sys_hash_md5:
    if (mbedtls_md5_finish((mbedtls_md5_context *)hash->ctx.ctx, hash->hash) ==
        0) {
      result = true;
    }
    mbedtls_md5_free((mbedtls_md5_context *)hash->ctx.ctx);
    break;
  case sys_hash_sha256:
    if (mbedtls_sha256_finish((mbedtls_sha256_context *)hash->ctx.ctx,
                              hash->hash) == 0) {
      result = true;
    }
    mbedtls_sha256_free((mbedtls_sha256_context *)hash->ctx.ctx);
    break;
  }

  // Set algorithm to zero to indicate finalization,
  // but leave size as is
  hash->algorithm = 0;

  // Clear the context and mark as finalized
  return result ? hash->hash : NULL;
}
