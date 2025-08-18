/**
 * @file NXPoint.h
 * @brief 2D point type and operations.
 *
 * This header defines NXPoint, a structure representing a point in 2D space.
 * It provides operations for creating, manipulating, and comparing points.
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief A structure representing a point in 2D space.
 * @ingroup Foundation
 * @headerfile NXPoint.h Foundation/Foundation.h
 */
typedef struct NXPoint {
  int32_t x; ///< The X coordinate of the point.
  int32_t y; ///< The Y coordinate of the point.
} NXPoint;

/**
 * @brief Compare two points for equality.
 *
 * Returns true when both X and Y coordinates are equal.
 *
 * @param a First point to compare.
 * @param b Second point to compare.
 * @return true if points are equal; false otherwise.
 */
static inline bool NXEqualsPoint(NXPoint a, NXPoint b) {
  return (a.x == b.x) && (a.y == b.y);
}

/**
 * @brief Create an NXPoint from integer coordinates.
 *
 * Convenience constructor for creating an `NXPoint` value. Implemented as a
 * header-only `static inline` function so it can be used without an external
 * symbol.
 *
 * @param x The X coordinate.
 * @param y The Y coordinate.
 * @return An `NXPoint` with the given coordinates.
 */
static inline NXPoint NXMakePoint(int32_t x, int32_t y) {
  NXPoint p;
  p.x = x;
  p.y = y;
  return p;
}
