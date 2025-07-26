/**
 * @file objc.h
 * @brief Defines the Objective-C runtime.
 * @defgroup objc Objective-C Runtime
 */
#pragma once

#include "assert.h"
#include "malloc.h"
#include "mutex.h"
#include "runtime.h"

#if __OBJC__

// Protocols
#include "NXConstantString+Protocol.h"
#include "Object+Protocol.h"

// Classes
#include "NXConstantString.h"
#include "Object.h"
#include "Protocol.h"

#endif // __OBJC__
