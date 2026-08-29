/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
#ifndef ROAD_H
#define ROAD_H

#include "raylib.h"
#include "graphics.h"
#include <stdbool.h>

// --------------------------------------------------
// Public API
// --------------------------------------------------

/*
 * Function: IsCarOnRoadSurface
 * --------------------------------
 * Checks whether a car (represented by a rectangular area) is entirely on a valid road surface.
 *
 * Parameters:
 *   pos - Center position of the car in world coordinates.
 *
 * Returns:
 *   true if all corners of the car are on valid road tiles; false otherwise.
 */
bool IsCarOnRoadSurface(Vector2 pos);

// --------------------------------------------------
// Internal helper functions (implemented as static in road.c)
// --------------------------------------------------
/*
 * These functions are NOT part of the public API and exist only in road.c.
 * Listed here for documentation purposes.
 */

/*
 * Function: PointInRect
 * --------------------------------
 * Determines if a point lies inside a rectangle.
 *
 * Parameters:
 *   p - Point to test.
 *   r - Rectangle.
 *
 * Returns:
 *   true if point p is inside rectangle r, false otherwise.
 */

/*
 * Function: IsOnRoadTile
 * --------------------------------
 * Checks whether a given position is inside the road area of a specific tile.
 *
 * Parameters:
 *   pos   - Position in world coordinates.
 *   t     - TileType of the tile.
 *   tileX - Top-left X coordinate of the tile in world coordinates.
 *   tileY - Top-left Y coordinate of the tile in world coordinates.
 *
 * Returns:
 *   true if the position is within the road area of the tile, false otherwise.
 */

#endif // ROAD_H
