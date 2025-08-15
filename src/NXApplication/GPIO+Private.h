/**
 * @file GPIO+Private.h
 * @brief GPIO class private header.
 */
#pragma once
#include <NXApplication/NXApplication.h>
#include <runtime-hw/hw.h>

/**
 * @brief Callback for GPIO events.
 */
void _gpio_callback(uint8_t pin, hw_gpio_event_t event);

/**
 * @brief Category for private methods of the GPIO class.
 */
@interface GPIO (Private)

/**
 * @brief Finalize all shared GPIO instances and clear the static table.
 * Call before application shutdown or when re-initializing GPIO.
 */
+ (void)finalize;

/**
 * @brief Calls the delegate's gpio:changed: method. If GPIO is subclassed,
 * the subclass's implementation will be called instead.
 */
- (void)changed:(GPIOEvent)event;

@end
