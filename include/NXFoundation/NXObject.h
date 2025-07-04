/**
 * @file NXObject.h
 * @brief Defines the NXObject class, the base class for the NXFoundation framework.
 * @details This file provides the definition for NXObject, which extends the root
 *          Object class with zone-based memory management capabilities.
 * 
 * \headerfile NXObject.h NXFoundation/NXFoundation.h
 */
#pragma once

/**
 * @brief The base class for objects in the NXFoundation framework.
 * @details NXObject extends the functionality of the root Object class by adding
 *          support for memory zones. All objects that are part of the NXFoundation
 *          framework should inherit from this class.
 */
@interface NXObject : Object {
@private
    /**
     * @var _zone
     * @brief The memory zone where the object is allocated.
     */
    id _zone;
}

/**
 * @brief Allocates a new instance of an object in a specific memory zone.
 * @param zone The NXZone in which to allocate the new instance.
 * @return A new instance of the receiving class, or nil if the allocation failed.
 */
+(id) allocWithZone:(NXZone* )zone;

/**
 * @brief Increases the retain count of the receiver.
 * @details This method is part of the reference counting memory management system.
 *          Sending a retain message to an object increases its retain count by one.
 * @return The receiver, with its retain count incremented.
 */
- (id) retain;

/**
 * @brief Decreases the retain count of the receiver.
 * @details This method is part of the reference counting memory management system.
 *          Sending a release message to an object decreases its retain count by one.
 *          If the retain count becomes zero, the object is deallocated.
 */
- (void) release;

@end
