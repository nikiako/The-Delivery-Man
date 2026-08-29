/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
// ===== file: menu.c =====
#include "menu.h"
#include "raylib.h"
#include <stdio.h>


int ShowLevelSelection(int unlockedLevel, int initialW, int initialH) {
    const int MAX_LEVELS = 25;
    if (unlockedLevel < 1) unlockedLevel = 1;
    if (unlockedLevel > MAX_LEVELS) unlockedLevel = MAX_LEVELS;

    int selectedLevel = unlockedLevel; // default selection is highest unlocked
    bool menuActive = true;

    const int gridCols = 5;
    const int gridRows = 5;
    const int cellW = 48;
    const int cellH = 36;
    const int gapX = 12;
    const int gapY = 12;
    const int gridTotalW = gridCols * cellW + (gridCols - 1) * gapX;
    const int gridTotalH = gridRows * cellH + (gridRows - 1) * gapY;
    const int startX = (initialW - gridTotalW) / 2;
    int startY = 0; // will be set below after header/icons

    int selCol = (selectedLevel - 1) % gridCols;
    int selRow = (selectedLevel - 1) / gridCols;

    // --- load icon textures (assets) ---
    Texture2D texMain = LoadTexture("assets/ai_car.png");
    Texture2D texAI   = LoadTexture("assets/main_car.png");

    // layout for header/icons
    const int titleY = 20;
    const int titleSize = 48;
    const float iconSize = 64.0f;
    const float iconGap = 20.0f;

    // Move icons and grid lower:
    const int extraVerticalOffset = 40;
    const int iconsY = titleY + titleSize + 8 + extraVerticalOffset; // moved down
    const int createdByY = iconsY + (int)iconSize + 12;
    startY = createdByY + 36 + extraVerticalOffset;

    while (menuActive && !WindowShouldClose()) {
        // input
        if (IsKeyPressed(KEY_RIGHT)) {
            selCol++;
            if (selCol >= gridCols) selCol = 0;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            selCol--;
            if (selCol < 0) selCol = gridCols - 1;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            selRow++;
            if (selRow >= gridRows) selRow = 0;
        }
        if (IsKeyPressed(KEY_UP)) {
            selRow--;
            if (selRow < 0) selRow = gridRows - 1;
        }

        // mouse selection
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            for (int r = 0; r < gridRows; ++r) {
                for (int c = 0; c < gridCols; ++c) {
                    int idx = r * gridCols + c; // zero-based index
                    int lvl = idx + 1;
                    int x = startX + c * (cellW + gapX);
                    int y = startY + r * (cellH + gapY);
                    Rectangle cell = { (float)x, (float)y, (float)cellW, (float)cellH };
                    if (CheckCollisionPointRec(m, cell)) {
                        if (lvl <= unlockedLevel) {
                            selectedLevel = lvl;
                            selCol = c;
                            selRow = r;
                            menuActive = false;
                            break;
                        }
                    }
                }
                if (!menuActive) break;
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            int idx = selRow * gridCols + selCol;
            int lvl = idx + 1;
            if (lvl <= unlockedLevel) {
                selectedLevel = lvl;
                menuActive = false;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            // signal quit (0) — avoid immediate return unload textures
            selectedLevel = 0;
            menuActive = false;
        }

        // wrap selection within existing levels (if sel points to > MAX_LEVELS, wrap)
        if (selRow * gridCols + selCol >= MAX_LEVELS) {
            selRow = 0; selCol = 0;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // --- Title (top) ---
        const char *title = "The Delivery Man";
        int titleW = MeasureText(title, titleSize);
        DrawText(title, initialW/2 - titleW/2, titleY, titleSize, DARKBLUE);

        // --- Icons row (car left, motorcycle right), centered ---
        float iconsTotalW = iconSize * 2.0f + iconGap;
        float iconsStartX = (initialW - iconsTotalW) / 2.0f;

        if (texMain.id != 0) {
            Rectangle src = { 0.0f, 0.0f, (float)texMain.width, (float)texMain.height };

            Rectangle dst = { iconsStartX, (float)iconsY, iconSize, iconSize };
            Vector2 origin = { dst.width/2.0f, dst.height/2.0f };
            DrawTexturePro(texMain, src, dst, origin, 0.0f, WHITE);
        }

        if (texAI.id != 0) {
            Rectangle src2 = { 0.0f, 0.0f, (float)texAI.width, (float)texAI.height };
            Rectangle dst2 = { iconsStartX + iconSize + iconGap, (float)iconsY, iconSize, iconSize };
            Vector2 origin2 = { dst2.width/2.0f, dst2.height/2.0f };
            DrawTexturePro(texAI, src2, dst2, origin2, 0.0f, WHITE);
        }

        // --- "Created by" text below icons ---
        const char *creator = "Created by Iakovidis Nikolaos";
        int creatorSize = 20;
        int creatorW = MeasureText(creator, creatorSize);
        DrawText(creator, initialW/2 - creatorW/2, createdByY, creatorSize, GRAY);

        // --- "Select Level" heading before grid  ---
        const char *selectHeading = "Select Level";
        int selectHSize = 28;
        int selectHW = MeasureText(selectHeading, selectHSize);
        DrawText(selectHeading, initialW/2 - selectHW/2, startY - 36, selectHSize, DARKGRAY);

        // draw grid
        Vector2 mouse = GetMousePosition();
        for (int r = 0; r < gridRows; ++r) {
            for (int c = 0; c < gridCols; ++c) {
                int idx = r * gridCols + c;
                int lvl = idx + 1;
                int x = startX + c * (cellW + gapX);
                int y = startY + r * (cellH + gapY);
                Rectangle rect = { (float)x, (float)y, (float)cellW, (float)cellH };
                Color bg = (lvl <= unlockedLevel) ? LIGHTGRAY : Fade(GRAY, 0.6f);
                DrawRectangleRec(rect, bg);
                DrawRectangleLines(x, y, cellW, cellH, DARKGRAY);

                char buf[8];
                sprintf(buf, "%d", lvl);
                int tw = MeasureText(buf, 18);
                DrawText(buf, x + (cellW - tw)/2, y + (cellH - 18)/2, 18, (lvl <= unlockedLevel) ? DARKGRAY : DARKGRAY);

                // hover highlight for selectable
                if (lvl <= unlockedLevel && CheckCollisionPointRec(mouse, rect)) {
                    DrawRectangleLines(x, y, cellW, cellH, YELLOW);
                }

                // selection marker
                if (selRow == r && selCol == c) {
                    DrawRectangleLines(x-2, y-2, cellW+4, cellH+4, BLUE);
                }
            }
        }

        // instructions & unlocked info (placed under grid)
        char info[64];
        sprintf(info, "Unlocked: %d   Use arrows / Enter or click level (Esc to quit)", unlockedLevel);
        DrawText(info, initialW/2 - MeasureText(info, 18)/2, startY + gridTotalH + 20, 18, DARKGRAY);

        const char *engAttribution = "This game was created for the course 'Structured Programming (2025-2026)' in ece auth.";
        int engSize = 16;
        int engW = MeasureText(engAttribution, engSize);
        DrawText(engAttribution, initialW/2 - engW/2, initialH - 36, engSize, DARKGRAY);

        EndDrawing();
    }

    // unload textures
    if (texMain.id != 0) UnloadTexture(texMain);
    if (texAI.id != 0) UnloadTexture(texAI);

    return selectedLevel;
}
