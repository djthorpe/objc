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
#include <string.h>

/**
 * @brief Initializes a new hash context for the specified algorithm.
 */
sys_hash_t sys_hash_init(sys_hash_algorithm_t algorithm) {
  sys_hash_t hash;
  hash.size = 0;
  hash.algorithm = 0; // Initialize to invalid state

  // Allocate EVP_MD_CTX dynamically and store pointer in union
  hash.ctx.ptr = EVP_MD_CTX_new();
  if (hash.ctx.ptr == NULL) {
    // Failed to allocate context
    return hash;
  }

  EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash.ctx.ptr;
  switch (algorithm) {
  case sys_hash_md5:
    if (EVP_DigestInit_ex(ctx, EVP_md5(), NULL)) {
      hash.size = 16; // MD5 produces a 128-bit hash
      hash.algorithm = algorithm;
    }
    break;
  case sys_hash_sha256:
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
      hash.size = 32; // SHA-256 produces a 256-bit hash
      hash.algorithm = algorithm;
    }
    break;
  }

  // If initialization failed, clean up
  if (hash.size == 0) {
    EVP_MD_CTX_free(ctx);
    hash.ctx.ptr = NULL;
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
  EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->ctx.ptr;
  return EVP_DigestUpdate(ctx, data, size) ? true : false;
}

/**
 * @brief Finalizes the hash computation and returns the hash value.
 */
const uint8_t *sys_hash_finalize(sys_hash_t *hash) {
  if (hash == NULL || hash->algorithm == 0 || hash->size == 0) {
    return NULL; // Invalid context
  }

  EVP_MD_CTX *ctx = (EVP_MD_CTX *)hash->ctx.ptr;
  bool result = EVP_DigestFinal_ex(ctx, hash->hash, NULL) ? true : false;
  EVP_MD_CTX_free(ctx); // Clean up the context
  hash->ctx.ptr = NULL;

  // Set algorithm to zero to indicate finalization
  hash->algorithm = 0;

  return result ? hash->hash : NULL;
}
