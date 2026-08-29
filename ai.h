/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
#ifndef AI_H
#define AI_H

#include "raylib.h"
#include "road.h"     // TileType, grid, IsCarOnRoadSurface
#include "graphics.h" // TILE_SIZE, GRID_W, GRID_H

// --------------------------------------------------
// struct AI car
// --------------------------------------------------
typedef struct {
    Vector2 position;
    float speed;
    Vector2 direction;
    int tileX;
    int tileY;
} AI_Car;

// --------------------------------------------------
// Public API functions
// --------------------------------------------------

/*
 * Function: UpdateAICar
 * --------------------------------
 * Updates AI car movement per frame according to road layout and approach behavior.
 *
 * Parameters:
 *   ai - Pointer to AI_Car instance to update.
 *   dt - Delta time (seconds) since last frame.
 *
 * Returns:
 *   None.
 */
void UpdateAICar(AI_Car *ai, float dt);

/*
 * Function: DirectionToAngle
 * --------------------------------
 * Converts a 2D direction vector to a rotation angle for rendering.
 *
 * Parameters:
 *   d - Direction vector (should be axis-aligned: ±1 in x or y).
 *
 * Returns:
 *   Rotation angle in degrees (0 = right, 90 = down, 180/-180 = left, -90 = up).
 */
float DirectionToAngle(Vector2 d);

// --------------------------------------------------
// Internal helper functions (implemented as static in ai.c)
// --------------------------------------------------
/*
 * These functions are NOT part of the public API and exist only in ai.c.
 * Listed here for documentation purposes.
 */

/*
 * Function: get_ai_state
 * --------------------------------
 * Retrieves or initializes AIState associated with a given AI_Car.
 *
 * Parameters:
 *   ai - Pointer to AI_Car instance.
 *
 * Returns:
 *   Pointer to the AIState corresponding to the AI_Car.
 */

/*
 * Function: IsOpposite
 * --------------------------------
 * Checks whether two direction vectors are exact opposites.
 *
 * Parameters:
 *   a - First direction vector.
 *   b - Second direction vector.
 *
 * Returns:
 *   true if vectors are opposite, false otherwise.
 */

/*
 * Function: ChooseDirection
 * --------------------------------
 * Chooses a new movement direction for a car based on tile type and current direction.
 *
 * Parameters:
 *   t            - TileType representing current tile.
 *   cur          - Current direction vector.
 *   allowReverse - If true, allows reversing direction.
 *
 * Returns:
 *   New direction vector for the AI car.
 */

/*
 * Function: MoveIntoExitLaneOnce
 * --------------------------------
 * Adjusts the car to align into the exit lane of a tile, with a small nudge.
 *
 * Parameters:
 *   ai    - Pointer to AI_Car instance.
 *   state - Pointer to AIState for the AI_Car.
 *
 * Returns:
 *   None.
 */

/*
 * Function: CorrectDirectionIfAtEdge
 * --------------------------------
 * Checks if the AI car would move off-road and corrects its direction if necessary.
 *
 * Parameters:
 *   ai - Pointer to AI_Car instance.
 *
 * Returns:
 *   None.
 */

/*
 * Function: ComputeApproachTargetClamped
 * --------------------------------
 * Computes a target position within the tile for smooth approach before applying planned direction.
 *
 * Parameters:
 *   ai               - Pointer to AI_Car instance.
 *   approachDistance - Distance to move along current direction before turning.
 *
 * Returns:
 *   Target world coordinates (Vector2) clamped inside the tile and right-hand lane.
 */

#endif // AI_H
