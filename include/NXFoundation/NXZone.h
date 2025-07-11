/**
 * @file NXZone.h
 * @brief Defines the NXZone class for memory management.
 *
 * This file provides the definition for NXZone, which is used to
 * manage memory allocations within specific regions, or "zones".
 */
#pragma once

/**
 * @brief A class for managing memory zones.
 *
 * NXZone provides a mechanism for managing memory allocations. It can
 * be used to create memory pools of a fixed size, from which objects can be
 * allocated. This can be useful for improving performance and reducing
 * memory fragmentation in applications that create and destroy many
 * objects.
 *
 * \headerfile NXZone.h NXFoundation/NXFoundation.h
 */
@interface NXZone : NXObject {
  /**
   * @var _size
   * @brief The size of the zone in bytes.
   *
   * If the size is 0, the zone is not limited in size and will grow as
   * needed.
   */
  size_t _size;

  /**
   * @var _data
   * @brief A pointer to the memory block managed by the zone.
   */
  void *_data; // Pointer to the allocated memory block

  /**
   * @var _count
   * @brief The number of active allocations.
   */
  size_t _count;
}

// Lifecycle
/**
 * @brief This method is unavailable for NXZone.
 *
 * Please use `+zoneWithSize:` to create a new zone.
 */
+ (id)alloc __attribute__((unavailable("Use +zoneWithSize: instead")));

/**
 * @brief This method is unavailable for NXZone.
 * @details Please use `+zoneWithSize:` to create a new zone.
 */
- (id)init __attribute__((unavailable("Use +zoneWithSize: instead")));

/**
 * @brief Returns the default memory zone.
 * @return The default NXZone instance.
 */
+ (id)defaultZone;

/**
 * @brief Creates a new memory zone with a specified size.
 * @param size The size of the memory zone in bytes.
 * @return A new NXZone instance, or nil if the allocation failed.
 */
+ (id)zoneWithSize:(size_t)size;

/**
 * @brief Deallocates the memory zone.
 * @details This frees the memory block managed by the zone.
 */
- (void)dealloc;

// Methods
/**
 * @brief Allocates a block of memory from the zone.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
- (void *)allocWithSize:(size_t)size;

/**
 * @brief Frees a block of memory that was allocated from the zone.
 * @param ptr A pointer to the memory block to be deallocated.
 */
- (void)free:(void *)ptr;

@end