/**
 * @file TimerDelegate+Protocol.h
 * @brief Defines a protocol for the timer delegate.
 *
 * The TimerDelegate protocol defines methods that are called by the
 * run loop when a Timer fires.
 */
#pragma once

/**
 * @protocol TimerDelegate
 * @ingroup Application
 * @headerfile TimerDelegate+Protocol.h Application/Application.h
 * @brief A protocol that defines the methods for a timer delegate.
 */
@protocol TimerDelegate

@required

/**
 * @brief Called when the timer fires.
 * @param timer The timer that fired.
 */
- (void)timerFired:(id)timer;

@end
