#pragma once
#include <objc/objc.h>

#if __OBJC__

// Forward declaration of classes
@class NXAutoreleasePool;
@class NXObject;
@class NXString;
@class NXThread;
@class NXZone;

// Includes
#include "NXObject.h"
#include "NXAutoreleasePool.h"
#include "NXLog.h"
#include "NXString.h"
#include "NXTimeInterval.h"
#include "NXThread.h"
#include "NXZone.h"
#include "Object+Description.h"

#endif // __OBJC__
