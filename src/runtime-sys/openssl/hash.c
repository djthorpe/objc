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

  // Store a pointer to the EVP_MD_CTX in our context buffer
  // We'll allocate it dynamically since EVP_MD_CTX is opaque in OpenSSL 3.x
  EVP_MD_CTX **ctx_ptr = (EVP_MD_CTX **)hash.ctx;
  *ctx_ptr = EVP_MD_CTX_new();

  if (*ctx_ptr == NULL) {
    // Failed to allocate context
    return hash;
  }

  switch (algorithm) {
  case sys_hash_md5:
    if (EVP_DigestInit_ex(*ctx_ptr, EVP_md5(), NULL)) {
      hash.size = 16; // MD5 produces a 128-bit hash
      hash.algorithm = algorithm;
    }
    break;
  case sys_hash_sha256:
    if (EVP_DigestInit_ex(*ctx_ptr, EVP_sha256(), NULL)) {
      hash.size = 32; // SHA-256 produces a 256-bit hash
      hash.algorithm = algorithm;
    }
    break;
  }

  // If initialization failed, clean up
  if (hash.size == 0) {
    EVP_MD_CTX_free(*ctx_ptr);
    *ctx_ptr = NULL;
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
  EVP_MD_CTX **ctx_ptr = (EVP_MD_CTX **)hash->ctx;
  return EVP_DigestUpdate(*ctx_ptr, data, size) ? true : false;
}

/**
 * @brief Finalizes the hash computation and returns the hash value.
 */
const uint8_t *sys_hash_finalize(sys_hash_t *hash) {
  if (hash == NULL || hash->algorithm == 0 || hash->size == 0) {
    return NULL; // Invalid context
  }

  EVP_MD_CTX **ctx_ptr = (EVP_MD_CTX **)hash->ctx;
  bool result = EVP_DigestFinal_ex(*ctx_ptr, hash->hash, NULL) ? true : false;
  EVP_MD_CTX_free(*ctx_ptr); // Clean up the context
  *ctx_ptr = NULL;

  // Set algorithm to zero to indicate finalization
  hash->algorithm = 0;

  return result ? hash->hash : NULL;
}
