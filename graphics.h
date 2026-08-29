/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"

// ================== GRID CONSTANTS ==================
#define TILE_SIZE 128
#define GRID_W 27
#define GRID_H 22

// ================== HOUSE CONSTANTS ==================
#define HOUSE_W (TILE_SIZE / 6)    // width of an upright house (and height for sideways)
#define HOUSE_H (TILE_SIZE / 10)   // roof height for upright / body width for sideways
#define HOUSE_MARGIN 6             // margin from tile edge
#define HOUSE_TOP_MARGIN 4         // vertical offset from top of tile

// ================== TILE TYPES ==================
typedef enum {
    EMPTY,
    HORIZONTAL,
    VERTICAL,
    T_UP,
    T_DOWN,
    T_LEFT,
    T_RIGHT,
    CROSS,
    TURN_UP_LEFT,
    TURN_UP_RIGHT,
    TURN_DOWN_LEFT,
    TURN_DOWN_RIGHT
} TileType;

// ================== GRID ==================
// Define as extern here; implement in graphics.c
extern TileType grid[GRID_H][GRID_W];

// ================== PUBLIC TILE & ROAD DRAW ==================
/*
 * Function: DrawTile
 * --------------------------------
 * Draws a tile of a given type at (x,y) with road and house coloring.
 *
 * Parameters:
 *   x, y      - Top-left coordinates of the tile in world space.
 *   t         - TileType of the tile to draw.
 *   roadMode  - Road color mode (0 = DARKGRAY, 1 = ORANGE).
 *   houseMode - House color mode (0 = RED, 1 = YELLOW).
 *
 * Returns:
 *   void
 */
void DrawTile(int x, int y, TileType t, int roadMode, int houseMode);

/*
 * Road drawing functions (specific T-junctions / turns)
 * --------------------------------
 * Each draws a specific road tile at (x,y) with road color mode.
 * Parameters:
 *   x, y      - Top-left coordinates of the tile.
 *   roadMode  - Road color mode (0 = DARKGRAY, 1 = ORANGE).
 * Returns:
 *   void
 */
void DrawTJunctionUp(int x, int y, int roadMode);
void DrawTJunctionDown(int x, int y, int roadMode);
void DrawTJunctionLeft(int x, int y, int roadMode);
void DrawTJunctionRight(int x, int y, int roadMode);
void DrawTurnUpLeft(int x, int y, int roadMode);
void DrawTurnUpRight(int x, int y, int roadMode);
void DrawTurnDownLeft(int x, int y, int roadMode);
void DrawTurnDownRight(int x, int y, int roadMode);

/*
 * House drawing functions
 * --------------------------------
 * Draw a house at the specified position with the given mode.
 * Parameters:
 *   x, y      - Top-left coordinates of the house.
 *   houseMode - 0 = RED, 1 = YELLOW.
 * Returns:
 *   void
 */
void DrawHouseUp(int x, int y, int houseMode);
void DrawHouseLeft(int x, int y, int houseMode);
void DrawHouseRight(int x, int y, int houseMode);

/*
 * House placement logic
 * --------------------------------
 * Draw two houses in a tile according to the tile type.
 * Parameters:
 *   tileX, tileY - Top-left coordinates of the tile.
 *   t            - TileType to determine house placement.
 *   houseMode    - 0 = RED, 1 = YELLOW.
 * Returns:
 *   void
 */
void DrawTwoHousesInTile(int tileX, int tileY, TileType t, int houseMode);

/*
 * Center line drawing functions
 * --------------------------------
 * Draw dashed lines to represent the center of roads.
 * Parameters:
 *   x, y   - Top-left coordinates.
 *   width/height (optional) - length of the line for boxed variants.
 * Returns:
 *   void
 */
void DrawCenterLineHorizontalFull(int x, int y);
void DrawCenterLineVerticalFull(int x, int y);
void DrawCenterLineHorizontalBox(int x, int y, int width);
void DrawCenterLineVerticalBox(int x, int y, int height);

// ================== INTERNAL / STATIC HELPERS ==================
/*
 * The following functions are implemented as static in graphics.c.
 * They are internal helpers and are listed here for documentation only.
 */

/*
 * Function: GetRoadColor
 * --------------------------------
 * Returns the Color corresponding to the road mode.
 * Parameters:
 *   mode - 0 = DARKGRAY, 1 = ORANGE
 * Returns:
 *   Color
 */

/*
 * Function: GetHouseColor
 * --------------------------------
 * Returns the Color corresponding to the house mode.
 * Parameters:
 *   mode - 0 = RED, 1 = YELLOW
 * Returns:
 *   Color
 */

/*
 * Function: DrawHousesAboveTile
 * --------------------------------
 * Draws multiple small houses above a horizontal road tile.
 * Parameters:
 *   x, y  - Top-left coordinates of tile
 *   count - Number of small houses to draw
 * Returns:
 *   void
 */

/*
 * Function: DrawHouseRoof
 * --------------------------------
 * Draws a decorative roof spanning the tile width (used for visuals only).
 * Parameters:
 *   x, y - Top-left coordinates of roof
 * Returns:
 *   void
 */

#endif // GRAPHICS_H

