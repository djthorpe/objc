/**
 * @file NXObject.h
 * @brief Defines the NXObject class, the base class for the NXFoundation
 * framework.
 *
 * This file provides the definition for NXObject, which extends the root
 * Object class with zone-based memory management capabilities.
 */
#pragma once

/**
 * @brief The base class for objects in the NXFoundation framework.
 *
 * NXObject extends the functionality of the root Object class by adding
 * support for memory zones, object retention and releasing. All objects
 * that are part of the NXFoundation framework inherit from this class.
 *
 * \headerfile NXObject.h NXFoundation/NXFoundation.h
 */
@interface NXObject : Object <RetainProtocol> {
@protected
  /**
   * @var _zone
   * @brief The memory zone where the object is allocated.
   */
  id _zone;

  /**
   * @var _retain
   * @brief The retain count of the object.
   */
  unsigned short _retain;

  /**
   * @var _next
   * @brief The next object in an autorelease pool.
   */
  id _next;
}

/**
 * @brief Allocates a new instance of an object in a specific memory zone.
 * @param zone The NXZone in which to allocate the new instance.
 * @return A new instance of the receiving class, or nil if the allocation
 * failed.
 */
+ (id)allocWithZone:(NXZone *)zone;

/**
 * @brief Increases the retain count of the receiver.
 * @return The receiver, with its retain count incremented.
 *
 * This method is part of the reference counting memory management system.
 * Sending a retain message to an object increases its retain count by one.
 */
- (id)retain;

/**
 * @brief Decreases the retain count of the receiver.
 *
 * This method is part of the reference counting memory management system.
 * Sending a release message to an object decreases its retain count by one.
 * If the retain count becomes zero, the object is deallocated.
 */
- (void)release;

/**
 * @brief Adds the receiver to the autorelease pool.
 */
- (id)autorelease;

@end
