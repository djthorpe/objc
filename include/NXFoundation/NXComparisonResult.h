/**
 * @file NXComparisonResult.h
 * @brief Defines the NXComparisonResult enumeration used for comparison
 * operations.
 */
#pragma once

///////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS

/**
 * @brief Enumeration representing the result of a comparison operation.
 *
 * NXComparisonResult is used throughout the NXFoundation framework to
 * represent the outcome of comparison operations between objects. The values
 * are designed to be compatible with standard C library comparison functions
 * and can be used directly in sorting algorithms.
 */
typedef enum {
  NXComparisonAscending = -1, ///< First operand is ordered before the second
  NXComparisonSame = 0,       ///< Operands are equivalent for ordering
  NXComparisonDescending = 1  ///< First operand is ordered after the second
} NXComparisonResult;
