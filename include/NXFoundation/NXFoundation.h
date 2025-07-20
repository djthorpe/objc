#pragma once
#include <objc/objc.h>

#if __OBJC__

// Forward declaration of classes
@class NXApplication;
@class NXAutoreleasePool;
@class NXObject;
@class NXString;
@class NXThread;
@class NXZone;

// Includes
#include "NXApplication.h"
#include "NXArch.h"
#include "NXAutoreleasePool.h"
#include "NXLog.h"
#include "NXNumber.h"
#include "NXObject.h"
#include "NXString+format.h"
#include "NXString.h"
#include "NXThread.h"
#include "NXTimeInterval.h"
#include "NXZone.h"
#include "Object+Description.h"

#endif // __OBJC__
