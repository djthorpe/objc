/**
 * @file timer.h
 * @brief Defines external timer functions.
 *
 * This file declares the opaque `objc_timer` type, and associated functions.
 */
#pragma once

typedef struct objc_timer objc_timer_t;

BOOL objc_timer_init(objc_timer_t* timer, unsigned int interval_ms, BOOL repeats,
                     void (*callback)(objc_timer_t* timer, void* context),
                     void* context);

void objc_timer_destroy(objc_timer_t* timer);
