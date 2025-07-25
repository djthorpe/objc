#pragma once
#include <objc/objc.h>

#if __OBJC__

// Forward Declaration of classes
@class NXApplication;
@class NXArray;
@class NXAutoreleasePool;
@class NXDate;
@class NXNull;
@class NXNumber;
@class NXObject;
@class NXString;
@class NXThread;
@class NXZone;

// Non-Class Definitions
#include "NXArch.h"
#include "NXComparisonResult.h"
#include "NXLog.h"
#include "NXTimeInterval.h"

// Protocols and Category Definitions
#include "JSON+Protocol.h"
#include "Object+Description.h"
#include "Retain+Protocol.h"

// Class Definitions
#include "NXApplication.h"
#include "NXArray.h"
#include "NXAutoreleasePool.h"
#include "NXDate.h"
#include "NXNull.h"
#include "NXNumber.h"
#include "NXObject.h"
#include "NXString.h"
#include "NXThread.h"
#include "NXZone.h"

#endif // __OBJC__
