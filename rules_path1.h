/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */

#ifndef RULES_PATH1_H
#define RULES_PATH1_H

#include "graphics.h"

/*
 * This module provides static map generation and a simple
 * deterministic path routing system based on road tiles.
 *
 * It operates on the global tile grid (grid[][]) and produces
 * a path mask (path[][]) that can be used by the game logic
 * and rendering system.
 */


/* ============================================================
 * mapstatic_rules
 * ============================================================
 * Summary:
 *   Generates a static road layout inside the active map area.
 *
 * Description:
 *   Initializes the tile grid with a deterministic road network
 *   consisting of borders, T-junctions, straight roads and
 *   intersections.
 *
 * Parameters:
 *   r - Number of active rows in the map.
 *   c - Number of active columns in the map.
 *
 * Return value:
 *   None.
 */
void mapstatic_rules(int r, int c);


/* ============================================================
 * path_r
 * ============================================================
 * Summary:
 *   Computes a deterministic L-shaped routing path.
 *
 * Description:
 *   Fills the global path[][] array with a non-optimal but valid
 *   route between two tile positions, based on road connectivity
 *   rules defined by tile_path1().
 *
 * Parameters:
 *   to_r   - Target tile row.
 *   to_c   - Target tile column.
 *   from_r - Starting tile row.
 *   from_c - Starting tile column.
 *
 * Return value:
 *   None.
 */
void path_r(int to_r, int to_c, int from_r, int from_c);


/* ============================================================
 * tile_path1
 * ============================================================
 * Summary:
 *   Checks whether movement through a tile is allowed.
 *
 * Description:
 *   Determines if a specific tile type allows horizontal or
 *   vertical traversal, according to predefined road rules.
 *
 * Parameters:
 *   from_r - Tile row to be checked.
 *   from_c - Tile column to be checked.
 *   how    - Direction of movement:
 *            1 = horizontal movement
 *            2 = vertical movement
 *
 * Return value:
 *   Returns 1 if traversal is allowed, 0 otherwise.
 */
int tile_path1(int from_r, int from_c, int how);


/* ============================================================
 * ClearGridOutsideActive
 * ============================================================
 * Summary:
 *   Clears tiles outside the active map region.
 *
 * Description:
 *   Sets all tiles outside the currently active map dimensions
 *   to EMPTY, ensuring unused grid areas are not rendered or
 *   used in gameplay logic.
 *
 * Parameters:
 *   rows - Number of active rows.
 *   cols - Number of active columns.
 *
 * Return value:
 *   None.
 */
void ClearGridOutsideActive(int rows, int cols);


/* ============================================================
 * path (global array)
 * ============================================================
 * Description:
 *   A global mask grid filled by path_r(), where:
 *     path[r][c] == 1 indicates that the routing path passes
 *     through tile (r, c).
 *
 * Usage:
 *   Read-only outside this module.
 */
extern short path[GRID_H][GRID_W];

#endif /* RULES_PATH1_H */
