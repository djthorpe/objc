/**
 * @file sleep.h
 * @brief Defines an external sleep function for the Objective-C runtime.
 * @details This file provides a simple function to pause the execution of the
 *          current thread for a specified duration.
 * 
 * \headerfile sleep.h objc/objc.h
 */
#pragma once

/**
 * @brief Pauses the execution of the current thread for a specified time.
 * @param milliseconds The number of milliseconds to sleep.
 */
void objc_sleep(unsigned int milliseconds);