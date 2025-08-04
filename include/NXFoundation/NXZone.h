/**
 * @file NXZone.h
 * @brief Defines the NXZone class for memory management.
 *
 * This file provides the definition for NXZone, which is used to
 * manage memory allocations within specific regions, or "zones".
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// NXZone

/**
 * @brief A class for managing memory zones.
 * @ingroup Foundation
 *
 * NXZone provides a mechanism for managing memory allocations. It can
 * be used to create memory arenas of a fixed size, from which objects can be
 * allocated.
 *
 * \headerfile NXZone.h NXFoundation/NXFoundation.h
 */
@interface NXZone : NXObject {
@protected
  size_t _size;  ///< Initial allocated size of the memory zone in bytes
  size_t _count; ///< Current number of active allocations in the zone
@private
  void *_root; ///< Root arena data pointer
  void *_cur;  ///< Current arena data pointer
}

/**
 * @brief Returns the default memory zone.
 * @return The default NXZone instance.
 */
+ (id)defaultZone;

/**
 * @brief Creates a new memory zone with a specified size.
 * @param size The size of the memory zone with initial capacity in bytes.
 * @return A new NXZone instance, or nil if the allocation failed.
 *
 * This method initializes a new memory zone with the specified capacity in
 * bytes. The zone will be expanded as needed to accommodate additional
 * requirements.
 */
+ (id)zoneWithSize:(size_t)size;

/**
 * @brief Deallocates the memory zone.
 * @details This frees the memory block managed by the zone.
 */
- (void)dealloc;

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

/**
 * @brief Walks through the zone and outputs information about allocations.
 *
 * This method iterates through all allocations in the zone and
 * prints their addresses and sizes to the log.
 */
- (void)dump;

/**
 * @brief Returns the total size of the memory zone.
 * @return The total size of the zone in bytes.
 *
 * This method returns the total capacity of the memory zone in bytes. Note that
 * this includes both used and free memory, plus the size of the NXZone
 * object itself.
 */
- (size_t)bytesTotal;

/**
 * @brief Returns the used size of the memory zone.
 * @return The used size of the zone in bytes.
 *
 * This method returns the number of used bytes in the memory zone.
 */
- (size_t)bytesUsed;

/**
 * @brief Returns the free size of the memory zone.
 * @return The free size of the zone in bytes.
 *
 * This method returns the number of free bytes in the memory zone.
 */
- (size_t)bytesFree;

@end