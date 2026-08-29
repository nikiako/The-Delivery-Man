/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
#include "road.h"

bool PointInRect(Vector2 p, Rectangle r) {
    return (p.x >= r.x && p.x <= r.x + r.width &&
            p.y >= r.y && p.y <= r.y + r.height);
}

bool IsOnRoadTile(Vector2 pos, TileType t, int tileX, int tileY) {
    int roadW = TILE_SIZE / 2;
    int offset = TILE_SIZE / 4;

    float left   = tileX + offset;
    float right  = tileX + offset + roadW;
    float top    = tileY + offset;
    float bottom = tileY + offset + roadW;

    switch (t) {
        case HORIZONTAL: return (pos.y >= top && pos.y <= bottom);
        case VERTICAL:   return (pos.x >= left && pos.x <= right);

        case T_UP:
            //  true  junction
            return (pos.y >= top && pos.y <= bottom) ||
                   (pos.x >= left && pos.x <= right && pos.y >= tileY + TILE_SIZE / 4);

        case T_DOWN:
            //  true  junction
            return (pos.y >= tileY + TILE_SIZE / 4 &&
                    pos.y <= tileY + TILE_SIZE * 3 / 4) ||
                   (pos.x >= left && pos.x <= right && pos.y <= tileY + TILE_SIZE * 3 / 4);


        case T_LEFT:
            return (pos.x >= left && pos.x <= right) ||
                   (pos.y >= top && pos.y <= bottom && pos.x >= tileX + TILE_SIZE / 2);

        case T_RIGHT:
            return (pos.x >= left && pos.x <= right) ||
                   (pos.y >= top && pos.y <= bottom && pos.x <= tileX + TILE_SIZE / 2);

        case CROSS:
            return (pos.y >= top && pos.y <= bottom) ||
                   (pos.x >= left && pos.x <= right);

        case TURN_UP_LEFT:
            return PointInRect(pos, (Rectangle){ tileX + offset, tileY, roadW, TILE_SIZE * 3 / 4 }) ||
                   PointInRect(pos, (Rectangle){ tileX, tileY + offset, TILE_SIZE / 2, roadW });

        case TURN_UP_RIGHT:
            return PointInRect(pos, (Rectangle){ tileX + offset, tileY, roadW, TILE_SIZE * 3 / 4 }) ||
                   PointInRect(pos, (Rectangle){ tileX + TILE_SIZE / 2, tileY + offset, TILE_SIZE / 2, roadW });

        case TURN_DOWN_LEFT:
            return PointInRect(pos, (Rectangle){ tileX + offset, tileY + offset, roadW, TILE_SIZE * 3 / 4 }) ||
                   PointInRect(pos, (Rectangle){ tileX, tileY + offset, TILE_SIZE / 2, roadW });

        case TURN_DOWN_RIGHT:
            return PointInRect(pos, (Rectangle){ tileX + offset, tileY + offset, roadW, TILE_SIZE * 3 / 4 }) ||
                   PointInRect(pos, (Rectangle){ tileX + TILE_SIZE / 2, tileY + offset, TILE_SIZE / 2, roadW });

        default: return false;
    }
}

bool IsCarOnRoadSurface(Vector2 pos) {
    float halfW = 12, halfH = 8;
    Vector2 pts[4] = {
        { pos.x - halfW, pos.y - halfH },
        { pos.x + halfW, pos.y - halfH },
        { pos.x - halfW, pos.y + halfH },
        { pos.x + halfW, pos.y + halfH }
    };

    for (int i = 0; i < 4; i++) {
        int mapX = (int)((pts[i].x - 50) / TILE_SIZE);
        int mapY = (int)((pts[i].y - 50) / TILE_SIZE);
        if (mapX < 0 || mapX >= GRID_W || mapY < 0 || mapY >= GRID_H)
            return false;

        if (!IsOnRoadTile(pts[i], grid[mapY][mapX],
            50 + mapX * TILE_SIZE,
            50 + mapY * TILE_SIZE))
            return false;
    }
    return true;
}
