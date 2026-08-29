/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
// ===== file: ui.c =====
#include "ui.h"
#include "raylib.h"

bool DrawMenuButtonAndCheckClick(int panelX, int panelY, int panelW, int panelH, bool panelAtBottom) {
    Rectangle menuBtn;
    if (panelAtBottom) {
        menuBtn = (Rectangle){ (float)(panelX + panelW - 90), (float)(panelY + 10), 80.0f, 30.0f };
    } else {
        // place near top-right inside right panel
        menuBtn = (Rectangle){ (float)(panelX + panelW - 90), 10.0f, 80.0f, 30.0f };
    }

    DrawRectangleRec(menuBtn, Fade(DARKGRAY, 0.35f));
    DrawRectangleLinesEx(menuBtn, 1, DARKGRAY);
    DrawText("MENU", (int)menuBtn.x + 18, (int)menuBtn.y + 7, 14, RAYWHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), menuBtn)) {
            return true;
        }
    }
    return false;
}
