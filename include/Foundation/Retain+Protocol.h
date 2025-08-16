/**
 * @file Retain+Protocol.h
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for retaining and releasing objects.
 * @ingroup Foundation
 *
 * The RetainProtocol defines the minimal interface that objects must
 * implement to provide retain and release functionality.
 *
 * \headerfile Retain+Protocol.h Foundation/Foundation.h
 */
@protocol RetainProtocol
@required
/**
 * @brief Increases the retain count of the receiver.
 * @return The receiver, with its retain count incremented.
 *
 * Sending a retain message to an object increases its retain count by one.
 */
- (id)retain;

/**
 * @brief Decreases the retain count of the receiver.
 *
 * Sending a release message to an object decreases its retain count by one.
 * If the retain count becomes zero, the object is deallocated.
 */
- (void)release;

@end
