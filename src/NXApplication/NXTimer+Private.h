/**
 * @file NXTimer+Private.h
 * @brief Timer class private header.
 */
#pragma once
#include <NXApplication/NXApplication.h>

/**
 * @brief Category for private methods of the Timer class.
 */
@interface NXTimer (Private)

/**
 * @brief Calls the delegate's timerFired: method. If NXTimer is subclassed,
 * the subclass's implementation will be called instead.
 */
- (void)timerFired;

@end
