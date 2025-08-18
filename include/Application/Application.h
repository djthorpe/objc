
/**
 * @file Application.h
 * @brief Application framework header
 * @defgroup Application Application Framework
 * @ingroup objc
 *
 * Application-related classes and types, including application lifecycle,
 * event handling, display and frames, and hardware interfaces.
 *
 * @example examples/Application/helloworld/main.m
 */
#pragma once
#include <Foundation/Foundation.h>
#include <runtime-hw/hw.h>

#if __OBJC__

// Forward Declaration of classes
@class Application;
@class GPIO;
@class NXInputManager;
@class NXTimer;

// Types and Enums
#import "NXApplication+Types.h"
#import "NXInputManager+Types.h"

// Protocols and Category Definitions
#include "ApplicationDelegate+Protocol.h"
#include "GPIODelegate+Protocol.h"
#include "InputManager+Protocols.h"
#include "TimerDelegate+Protocol.h"

// Class Definitions
#include "GPIO.h"
#include "LED.h"
#include "NXApplication.h"
#include "NXApplicationMain.h"
#include "NXTimer.h"

#endif // __OBJC__
