#include <NXFoundation/NXFoundation.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/** @brief Base unit: 1 millisecond */
const NXTimeInterval Millisecond = 1;

/** @brief 1 second = 1,000 milliseconds */
const NXTimeInterval Second = 1000 * Millisecond;

/** @brief 1 minute = 60,000 milliseconds */
const NXTimeInterval Minute = 60 * Second;

/** @brief 1 hour = 3,600,000 milliseconds */
const NXTimeInterval Hour = 60 * Minute;
