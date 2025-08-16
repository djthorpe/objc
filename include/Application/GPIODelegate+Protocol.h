/**
 * @file GPIODelegate+Protocol.h
 * @brief Defines a protocol for the GPIO delegate.
 *
 * The GPIODelegate protocol defines methods that are called by the
 * run loop when a GPIO event occurs.
 */
#pragma once
#include "GPIOTypes.h"

/**
 * @protocol GPIODelegate
 * @ingroup Application
 * @headerfile GPIODelegate+Protocol.h Application/Application.h
 * @brief A protocol that defines the methods for a GPIO delegate.
 */
@protocol GPIODelegate

@required

/**
 * @brief Called when the GPIO input changes.
 * @param sender The GPIO that triggered the event.
 * @param event The event that occurred.
 *
 * This method is called when the GPIO input changes, either GPIOEventRising
 * (from low to high) or GPIOEventFalling (from high to low). When both edges
 * are detected, the event will include both GPIOEventRising and
 * GPIOEventFalling (GPIOEventChanged).
 */
- (void)gpio:(id)sender changed:(GPIOEvent)event;

@end
