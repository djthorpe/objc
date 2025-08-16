
/**
 * @file NXNull.h
 * @brief Defines a null value that can be inserted into collections.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS

/**
 * @brief The NXNull class
 * @ingroup Foundation
 *
 * NXNull represents a null value that can be inserted into collections.
 *
 * \headerfile NXNull.h Foundation/Foundation.h
 */
@interface NXNull : NXObject <JSONProtocol>

/**
 * @brief Returns the shared singleton instance of NXNull.
 * @return The shared NXNull instance.
 */
+ (NXNull *)nullValue;

@end
