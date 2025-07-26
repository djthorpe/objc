/**
 * @file hash.c
 * @brief Implements hash generation functionality using OpenSSL.
 *
 * This file implements various system methods for hash generation.
 */
#include <openssl/evp.h>
#include <runtime-sys/hash.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Initializes a new hash context for the specified algorithm.
 */
sys_hash_t sys_hash_init(sys_hash_algorithm_t algorithm) {
  sys_hash_t hash;
  hash.size = 0;
  hash.ctx = EVP_MD_CTX_new();

  if (hash.ctx == NULL) {
    // Failed to allocate context
    return hash;
  }

  switch (algorithm) {
  case sys_hash_md5:
    if (EVP_DigestInit_ex(hash.ctx, EVP_md5(), NULL)) {
      hash.size = 16; // MD5 produces a 128-bit hash
    }
    break;
  case sys_hash_sha256:
    if (EVP_DigestInit_ex(hash.ctx, EVP_sha256(), NULL)) {
      hash.size = 32; // SHA-256 produces a 256-bit hash
    }
    break;
  }

  // If initialization failed, clean up and reset
  if (hash.size == 0) {
    EVP_MD_CTX_free(hash.ctx);
    hash.ctx = NULL;
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
  if (data == NULL || hash == NULL || hash->ctx == NULL) {
    return false;
  }
  if (size == 0) {
    return true; // No data to update, consider it a success
  }
  return EVP_DigestUpdate(hash->ctx, data, size) ? true : false;
}

/**
 * @brief Finalizes the hash computation and returns the hash value.
 */
const uint8_t *sys_hash_finalize(sys_hash_t *hash) {
  bool result = false;
  if (hash && hash->ctx) {
    result = EVP_DigestFinal_ex(hash->ctx, hash->hash, NULL) ? true : false;
    EVP_MD_CTX_free(hash->ctx); // Clean up the context
    hash->ctx = NULL;
    // Don't reset size - leave it so caller knows the hash length
  }
  return result ? hash->hash : NULL;
}
