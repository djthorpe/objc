/**
 * @file Object+Protocol.h
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// PROTOCOL DEFINITIONS

/**
 * @brief Protocol for objects.
 *
 * The ObjectProtocol defines the minimal interface that objects must
 * implement to provide basic functionality.
 *
 * \headerfile Object+Protocol.h NXFoundation/NXFoundation.h
 */
@protocol ObjectProtocol

@required
- (Class)class;
- (BOOL)isKindOfClass:(Class)aClass;

@end
