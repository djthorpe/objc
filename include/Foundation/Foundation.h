/**
 * @file Foundation.h
 * @brief Foundation framework header
 * @defgroup Foundation Foundation Framework
 * @ingroup objc
 *
 * Foundational classes and types, including strings, numbers, dates, data and
 * collections.
 */
#pragma once
#include <objc/objc.h>

#if __OBJC__

// Forward Declaration of classes
@class NXArray;
@class NXAutoreleasePool;
@class NXData;
@class NXDate;
@class NXNull;
@class NXNumber;
@class NXMap;
@class NXObject;
@class NXString;
@class NXThread;
@class NXZone;

// Non-Class Definitions
#include "NXArch.h"
#include "NXComparisonResult.h"
#include "NXLog.h"
#include "NXNotFound.h"
#include "NXPoint.h"
#include "NXTimeInterval.h"

// Protocols and Category Definitions
#include "Collection+Protocol.h"
#include "JSON+Protocol.h"
#include "Object+Description.h"
#include "Retain+Protocol.h"

// Class Definitions
#include "NXArray.h"
#include "NXAutoreleasePool.h"
#include "NXData.h"
#include "NXDate.h"
#include "NXMap.h"
#include "NXNull.h"
#include "NXNumber.h"
#include "NXObject.h"
#include "NXString.h"
#include "NXThread.h"
#include "NXZone.h"

#endif // __OBJC__
