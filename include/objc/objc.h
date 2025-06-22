#pragma once
#include "runtime.h"
#include "panicf.h"

#if __OBJC__
#include "Object.h"
#include "Protocol.h"
#endif // __OBJC__

// Initializes the Objective-C runtime and registers the modules
void objc_init();
