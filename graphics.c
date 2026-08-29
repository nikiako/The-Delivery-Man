/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
#include "graphics.h"


// ================== HELPER: map modes to colors ==================
static Color GetRoadColor(int mode) {
    return (mode == 1) ? ORANGE : DARKGRAY; // 0 -> DARKGRAY, 1 -> ORANGE
}

static Color GetHouseColor(int mode) {
    return (mode == 1) ? YELLOW : RED; // 0 -> RED, 1 -> YELLOW
}

// ================== OPTIONAL: helper to draw multiple small houses above a road (kept from original) ==================
// Draw small houses above a tile with a horizontal road (kept for compatibility)
void DrawHousesAboveTile(int x, int y, int count) {
    if (count <= 0) return;

    int roadW = TILE_SIZE / 2;
    int houseWidth = TILE_SIZE / (count * 2 + 1); // house width depending on count
    int houseHeight = TILE_SIZE / 4;              // roof height
    int spacing = houseWidth / 2;                 // spacing between houses
    int houseY = y + roadW / 2 - houseHeight - 2; // above the road

    for (int i = 0; i < count; i++) {
        int houseX = x + spacing + i * (houseWidth + spacing);

        // roof
        Vector2 p1 = { (float)houseX, (float)(houseY + houseHeight) };
        Vector2 p2 = { (float)(houseX + houseWidth / 2), (float)houseY };
        Vector2 p3 = { (float)(houseX + houseWidth), (float)(houseY + houseHeight) };
        DrawTriangle(p1, p2, p3, MAROON);

        // house body
        DrawRectangle(houseX, houseY + houseHeight, houseWidth, houseHeight, BEIGE);
    }
}

// ================== CENTER LINES ==================
void DrawCenterLineHorizontalFull(int x, int y) {
    int dash = 16, gap = 16;
    int lineY = y + TILE_SIZE / 4;
    for (int i = 0; i < TILE_SIZE; i += dash + gap)
        DrawRectangle(x + i, lineY - 2, dash, 4, WHITE);
}

void DrawCenterLineVerticalFull(int x, int y) {
    int dash = 16, gap = 16;
    int lineX = x + TILE_SIZE / 4;
    for (int i = 0; i < TILE_SIZE; i += dash + gap)
        DrawRectangle(lineX - 2, y + i, 4, dash, WHITE);
}

void DrawCenterLineHorizontalBox(int x, int y, int width) {
    int dash = 16, gap = 16;
    int lineY = y + width / 2;
    for (int i = 0; i < width; i += dash + gap)
        DrawRectangle(x + i, lineY - 2, dash, 4, WHITE);
}

void DrawCenterLineVerticalBox(int x, int y, int height) {
    int dash = 16, gap = 16;
    int lineX = x + height / 2;
    for (int i = 0; i < height; i += dash + gap)
        DrawRectangle(lineX - 2, y + i, 4, dash, WHITE);
}

// ================== T-JUNCTIONS ==================
void DrawTJunctionUp(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;
    int offset = TILE_SIZE / 4;
    int verticalHeight = TILE_SIZE / 2;

    DrawRectangle(x + offset, y + verticalHeight, roadW,
                  TILE_SIZE - verticalHeight, roadColor);
    DrawCenterLineVerticalFull(x + offset, y + verticalHeight);

    DrawRectangle(x, y + offset, TILE_SIZE, roadW, roadColor);
    DrawCenterLineHorizontalFull(x, y + offset);
}

void DrawTJunctionDown(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;
    int offset = TILE_SIZE / 4;
    int verticalHeight = TILE_SIZE / 2;

    DrawRectangle(x + offset, y, roadW, verticalHeight, roadColor);


    DrawRectangle(x, y + TILE_SIZE - roadW - offset,
                  TILE_SIZE, roadW, roadColor);
    DrawCenterLineHorizontalFull(x, y + TILE_SIZE - roadW - offset);
}

void DrawTJunctionLeft(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;
    int offset = TILE_SIZE / 4;
    int horizontalWidth = TILE_SIZE / 2;

    DrawRectangle(x + offset, y, roadW, TILE_SIZE, roadColor);
    DrawCenterLineVerticalFull(x + offset, y);

    DrawRectangle(x + TILE_SIZE - horizontalWidth + offset,
                  y + offset, horizontalWidth, roadW, roadColor);
    DrawCenterLineHorizontalFull(
        x + TILE_SIZE - horizontalWidth + offset,
        y + offset);
}

void DrawTJunctionRight(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;
    int offset = TILE_SIZE / 4;
    int horizontalWidth = TILE_SIZE / 2;

    DrawRectangle(x + TILE_SIZE - roadW - offset,
                  y, roadW, TILE_SIZE, roadColor);
    DrawCenterLineVerticalFull(
        x + TILE_SIZE - roadW - offset, y);

    DrawRectangle(x - offset, y + offset,
                  horizontalWidth, roadW, roadColor);

}

// ================== TURN TILES ==================
void DrawTurnUpLeft(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;

    DrawRectangle(x + TILE_SIZE / 4, y,
                  roadW, TILE_SIZE * 3 / 4, roadColor);
    DrawCenterLineVerticalBox(
        x + TILE_SIZE / 4,
        y + TILE_SIZE / 4,
        TILE_SIZE / 2);

    DrawRectangle(x, y + TILE_SIZE / 4,
                  TILE_SIZE / 2, roadW, roadColor);
    DrawCenterLineHorizontalBox(
        x, y + TILE_SIZE / 4,
        TILE_SIZE / 2);
}

void DrawTurnUpRight(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;

    DrawRectangle(x + TILE_SIZE / 4, y,
                  roadW, TILE_SIZE * 3 / 4, roadColor);
    DrawCenterLineVerticalBox(
        x + TILE_SIZE / 4,
        y + TILE_SIZE / 4,
        TILE_SIZE / 2);

    DrawRectangle(x + TILE_SIZE / 2, y + TILE_SIZE / 4,
                  TILE_SIZE / 2, roadW, roadColor);
    DrawCenterLineHorizontalBox(
        x + TILE_SIZE / 2, y + TILE_SIZE / 4,
        TILE_SIZE / 2);
}

void DrawTurnDownLeft(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;

    DrawRectangle(x + TILE_SIZE / 4, y + TILE_SIZE / 4,
                  roadW, TILE_SIZE * 3 / 4, roadColor);
    DrawCenterLineVerticalBox(
        x + TILE_SIZE / 4,
        y + TILE_SIZE / 2,
        TILE_SIZE / 2);

    DrawRectangle(x, y + TILE_SIZE / 4,
                  TILE_SIZE / 2, roadW, roadColor);
    DrawCenterLineHorizontalBox(
        x, y + TILE_SIZE / 4,
        TILE_SIZE / 2);
}

void DrawTurnDownRight(int x, int y, int roadMode) {
    Color roadColor = GetRoadColor(roadMode);
    int roadW = TILE_SIZE / 2;

    DrawRectangle(x + TILE_SIZE / 4, y + TILE_SIZE / 4,
                  roadW, TILE_SIZE * 3 / 4, roadColor);
    DrawCenterLineVerticalBox(
        x + TILE_SIZE / 4,
        y + TILE_SIZE / 2,
        TILE_SIZE / 2);

    DrawRectangle(x + TILE_SIZE / 2, y + TILE_SIZE / 4,
                  TILE_SIZE / 2, roadW, roadColor);
    DrawCenterLineHorizontalBox(
        x + TILE_SIZE / 2, y + TILE_SIZE / 4,
        TILE_SIZE / 2);
}

// ================== HOUSE DRAWING  ==================

// Upright house: roof on top, body below
void DrawHouseUp(int x, int y, int houseMode) {
    Color houseColor = GetHouseColor(houseMode);
    Vector2 p1 = { (float)x, (float)(y + HOUSE_H) };                    // roof base left
    Vector2 p2 = { (float)(x + HOUSE_W / 2.0f), (float)y };             // roof apex
    Vector2 p3 = { (float)(x + HOUSE_W), (float)(y + HOUSE_H) };          // roof base right
    DrawTriangle(p1, p2, p3, houseColor);
    DrawRectangle(x, y + HOUSE_H, HOUSE_W, HOUSE_H, houseColor);
}

// Sideways house facing right (roof points right)
void DrawHouseRight(int x, int y, int houseMode) {
    Color houseColor = GetHouseColor(houseMode);
    int bodyW = HOUSE_H;    // narrow body
    int bodyH = HOUSE_W;    // tall body

    // body
    DrawRectangle(x, y, bodyW, bodyH, houseColor);

    // roof pointing right
    Vector2 p1 = { (float)(x + bodyW), (float)y };
    Vector2 p2 = { (float)(x + bodyW + HOUSE_H), (float)(y + bodyH / 2.0f) };
    Vector2 p3 = { (float)(x + bodyW), (float)(y + bodyH) };
    DrawTriangle(p1, p2, p3, houseColor);
}

// Sideways house facing left (roof points left)
void DrawHouseLeft(int x, int y, int houseMode) {
    Color houseColor = GetHouseColor(houseMode);
    int bodyW = HOUSE_H;
    int bodyH = HOUSE_W;

    int bodyX = x + HOUSE_H;
    // body
    DrawRectangle(bodyX, y, bodyW, bodyH, houseColor);

    // roof pointing left
    Vector2 p1 = { (float)bodyX, (float)y };
    Vector2 p2 = { (float)x, (float)(y + bodyH / 2.0f) };
    Vector2 p3 = { (float)bodyX, (float)(y + bodyH) };
    DrawTriangle(p1, p2, p3, houseColor);
}

// ================== HOUSE PLACEMENT LOGIC ==================
// See earlier description for mapping of tile types to house placement
void DrawTwoHousesInTile(int tileX, int tileY, TileType t, int houseMode) {
    if (t == EMPTY) return;

    // Vertical placements
    int topY = tileY + HOUSE_TOP_MARGIN;
    int bottomY_upright = tileY + TILE_SIZE - HOUSE_MARGIN - 2 * HOUSE_H; // for upright stacked
    int bottomY_sideways = tileY + TILE_SIZE - HOUSE_MARGIN - HOUSE_W;    // for sideways

    // Horizontal placements
    int leftX = tileX + HOUSE_MARGIN;
    int rightX = tileX + TILE_SIZE - HOUSE_MARGIN - HOUSE_W; // for upright, right side
    int sidewaysTotalW = 2 * HOUSE_H; // body + roof for sideways

    switch(t) {
        case VERTICAL: {
            int posX = tileX + HOUSE_MARGIN;
            DrawHouseLeft(posX, topY, houseMode);
            DrawHouseLeft(posX, bottomY_sideways, houseMode);
            break;
        }

        case TURN_UP_LEFT:
        case TURN_DOWN_LEFT:
            // stacked upright houses on left side
            DrawHouseUp(leftX, topY, houseMode);
            DrawHouseUp(leftX, bottomY_upright, houseMode);
            break;

        case TURN_UP_RIGHT:
        case TURN_DOWN_RIGHT:
            // stacked upright houses on right side
            DrawHouseUp(rightX, topY, houseMode);
            DrawHouseUp(rightX, bottomY_upright, houseMode);
            break;

        case T_LEFT:
            // sideways houses left
            DrawHouseLeft(leftX, topY, houseMode);
            DrawHouseLeft(leftX, bottomY_sideways, houseMode);
            break;

        case T_RIGHT:
            // sideways houses right
            DrawHouseRight(tileX + TILE_SIZE - HOUSE_MARGIN - sidewaysTotalW, topY, houseMode);
            DrawHouseRight(tileX + TILE_SIZE - HOUSE_MARGIN - sidewaysTotalW, bottomY_sideways, houseMode);
            break;

        default:
            // default: upright houses top-left and top-right
            DrawHouseUp(leftX, topY, houseMode);
            DrawHouseUp(rightX, topY, houseMode);
            break;
    }
}

// ================== DECORATION  ==================
void DrawHouseRoof(int x, int y) {
    int roofHeight = TILE_SIZE / 2;
    int roofWidth  = TILE_SIZE;

    Vector2 p1 = { (float)x, (float)(y + roofHeight) };
    Vector2 p2 = { (float)(x + roofWidth / 2), (float)y };
    Vector2 p3 = { (float)(x + roofWidth), (float)(y + roofHeight) };

    DrawTriangle(p1, p2, p3, MAROON);
}

// ================== TILE DRAW ==================
void DrawTile(int x, int y, TileType t, int roadMode, int houseMode) {
    int roadW = TILE_SIZE / 2;

    switch (t) {
        case HORIZONTAL: {
            Color roadColor = GetRoadColor(roadMode);
            DrawRectangle(x, y + TILE_SIZE / 4,
                          TILE_SIZE, roadW, roadColor);
            DrawCenterLineHorizontalFull(x, y + TILE_SIZE / 4);
            break;
        }

        case VERTICAL: {
            Color roadColor = GetRoadColor(roadMode);
            DrawRectangle(x + TILE_SIZE / 4, y,
                          roadW, TILE_SIZE, roadColor);
            DrawCenterLineVerticalFull(x + TILE_SIZE / 4, y);
            break;
        }

        case T_UP:    DrawTJunctionUp(x, y, roadMode); break;
        case T_DOWN:  DrawTJunctionDown(x, y, roadMode); break;
        case T_LEFT:  DrawTJunctionLeft(x, y, roadMode); break;
        case T_RIGHT: DrawTJunctionRight(x, y, roadMode); break;

        case CROSS: {
            Color roadColor = GetRoadColor(roadMode);
            DrawRectangle(x + TILE_SIZE / 4, y,
                          TILE_SIZE / 2, TILE_SIZE, roadColor);
            DrawCenterLineVerticalFull(
                x + TILE_SIZE / 4, y);

            DrawRectangle(x, y + TILE_SIZE / 4,
                          TILE_SIZE, TILE_SIZE / 2, roadColor);
            DrawCenterLineHorizontalFull(
                x, y + TILE_SIZE / 4);
            break;
        }

        case TURN_UP_LEFT:    DrawTurnUpLeft(x, y, roadMode); break;
        case TURN_UP_RIGHT:   DrawTurnUpRight(x, y, roadMode); break;
        case TURN_DOWN_LEFT:  DrawTurnDownLeft(x, y, roadMode); break;
        case TURN_DOWN_RIGHT: DrawTurnDownRight(x, y, roadMode); break;

        default:
            break;
    }

    // draw houses according to tile type (placement logic above)
    DrawTwoHousesInTile(x, y, t, houseMode);
}
