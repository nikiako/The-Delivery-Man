// ===== file: main.c =====
/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"

#include "graphics.h"
#include "road.h"
#include "ai.h"
#include "rules.h"

#include "save.h"
#include "menu.h"
#include "ui.h"
#include "rules_path1.h"

#define PANEL_WIDTH 300
#define PANEL_HEIGHT_BOTTOM 160
#define MAX_PATH (GRID_H * GRID_W)
#define MAX_LEVELS 25

TileType grid[GRID_H][GRID_W];

int ACTIVE_MAP_ROWS = 0;
int ACTIVE_MAP_COLS = 0;

typedef struct {
    Vector2 position;
    float speed;
    Vector2 direction;
} Car;

#define MAX_AI 100


/* Helper: convert a world position to tile coordinates */
static void PosToTileCoords(Vector2 p, int mapOriginX, int mapOriginY, int *r, int *c) {
    int col = (int)((p.x - mapOriginX) / TILE_SIZE);
    int row = (int)((p.y - mapOriginY) / TILE_SIZE);
    if (row < 0) row = 0;
    if (col < 0) col = 0;
    if (row >= ACTIVE_MAP_ROWS) row = ACTIVE_MAP_ROWS - 1;
    if (col >= ACTIVE_MAP_COLS) col = ACTIVE_MAP_COLS - 1;
    *r = row; *c = col;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    int baseRows = GRID_H / 3;
    int baseCols = GRID_W / 3;
    if (baseRows < 4) baseRows = 4;
    if (baseCols < 6) baseCols = 6;
    if (baseRows > GRID_H) baseRows = GRID_H;
    if (baseCols > GRID_W) baseCols = GRID_W;

    int initialW = 1300;
    int initialH = 900;
    InitWindow(initialW, initialH, "Delivery Game");
    SetTargetFPS(60);

    Texture2D mainCarTexture = LoadTexture("assets/main_car.png");
    Texture2D aiCarTexture   = LoadTexture("assets/ai_car.png");

    bool quitAll = false;

    while (!quitAll && !WindowShouldClose()) {
        int savedLevel = LoadSavedLevel();
        int unlockedLevel = (savedLevel > 0) ? savedLevel : 1;
        if (unlockedLevel > MAX_LEVELS) unlockedLevel = MAX_LEVELS;

        int startLevel = ShowLevelSelection(unlockedLevel, initialW, initialH);
        if (startLevel <= 0) break;

        mapstatic_rules(baseRows + (startLevel - 1), baseCols + (startLevel - 1));
        ACTIVE_MAP_ROWS = baseRows + (startLevel - 1);
        ACTIVE_MAP_COLS = baseCols + (startLevel - 1);
        if (ACTIVE_MAP_ROWS > GRID_H) ACTIVE_MAP_ROWS = GRID_H;
        if (ACTIVE_MAP_COLS > GRID_W) ACTIVE_MAP_COLS = GRID_W;

        ClearGridOutsideActive(ACTIVE_MAP_ROWS, ACTIVE_MAP_COLS);

        Car player = {{50 + TILE_SIZE/2, 50 + TILE_SIZE/2}, 200.0f, {1.0f, 0.0f}};

        GameRules rules;
        InitGameRules(&rules);

        StartLevel(&rules, startLevel);
        SaveLevelToFile(startLevel);

        int ordersPage = 0;
        const int ORDERS_PER_PAGE = 8;

        int pathTiles[MAX_PATH][2];
        bool pathGrid[GRID_H][GRID_W];

        const int mapOriginX = 50;
        const int mapOriginY = 50;

        int aiCount = 3 + (startLevel - 1) * 2;
        if (aiCount > MAX_AI) aiCount = MAX_AI;
        AI_Car aiCars[MAX_AI];

        {
            bool used[GRID_H][GRID_W];
            for (int rr = 0; rr < GRID_H; ++rr)
                for (int cc = 0; cc < GRID_W; ++cc)
                    used[rr][cc] = false;

            int pcol = (int)((player.position.x - mapOriginX) / TILE_SIZE);
            int prow = (int)((player.position.y - mapOriginY) / TILE_SIZE);
            if (prow < 0) prow = 0;
            if (pcol < 0) pcol = 0;
            if (prow >= ACTIVE_MAP_ROWS) prow = ACTIVE_MAP_ROWS - 1;
            if (pcol >= ACTIVE_MAP_COLS) pcol = ACTIVE_MAP_COLS - 1;
            used[prow][pcol] = true;

            for (int i = 0; i < aiCount; ++i) {
                bool placed = false;
                int attempts = 0;
                while (!placed && attempts < 2000) {
                    attempts++;
                    int rr = rand() % ACTIVE_MAP_ROWS;
                    int cc = rand() % ACTIVE_MAP_COLS;
                    if (used[rr][cc]) continue;

                    float px = (float)(mapOriginX + cc * TILE_SIZE + TILE_SIZE/2);
                    float py = (float)(mapOriginY + rr * TILE_SIZE + TILE_SIZE/2);
                    Vector2 pos = { px, py };

                    if (!IsCarOnRoadSurface(pos)) continue;

                    aiCars[i] = (AI_Car){{px, py}, 150.0f, {1.0f, 0.0f}, 0, 0};
                    used[rr][cc] = true;
                    placed = true;
                }

                if (!placed) {
                    for (int rr = 0; rr < ACTIVE_MAP_ROWS && !placed; ++rr) {
                        for (int cc = 0; cc < ACTIVE_MAP_COLS && !placed; ++cc) {
                            if (used[rr][cc]) continue;
                            float px = (float)(mapOriginX + cc * TILE_SIZE + TILE_SIZE/2);
                            float py = (float)(mapOriginY + rr * TILE_SIZE + TILE_SIZE/2);
                            Vector2 pos = { px, py };
                            if (!IsCarOnRoadSurface(pos)) continue;
                            aiCars[i] = (AI_Car){{px, py}, 150.0f, {1.0f, 0.0f}, 0, 0};
                            used[rr][cc] = true;
                            placed = true;
                        }
                    }
                }

                if (!placed) {
                    float px = player.position.x + TILE_SIZE;
                    float py = player.position.y + TILE_SIZE;
                    aiCars[i] = (AI_Car){{px, py}, 150.0f, {1.0f, 0.0f}, 0, 0};
                }
            }
        }

        Camera2D camera = { 0 };
        camera.zoom = 1.0f;

        bool panelVisible = true;
        bool panelAtBottom = false;
        bool fullscreen = false;

        bool returnToMenu = false;

        bool collided = false;
        float collideTimer = 0.0f;
        const float COLLIDE_SHOW_TIME = 1.5f;

        float levelTimeRemaining = 120.0f;
        float levelFailTimer = 0.0f;

        const float PAUSE_OFFSET_Y = 30.0f;

        // NOTE: The "Lose On Exit" toggle has been removed. Instead of failing
        // immediately when leaving the map or going off-road, we give a small
        // tolerance window. The timer below accumulates time off-road/outside
        // bounds; if it exceeds EXIT_OFFROAD_TOLERANCE the level fails.
        float exitOffRoadTimer = 0.0f;
        const float EXIT_OFFROAD_TOLERANCE = 2.0f;

        while (!WindowShouldClose()) {
            float dt = GetFrameTime();

            if (IsKeyPressed(KEY_TAB)) panelVisible = !panelVisible;
            if (IsKeyPressed(KEY_F11)) {
                fullscreen = !fullscreen;
                ToggleFullscreen();
            }

            int mapPixelWidth  = ACTIVE_MAP_COLS * TILE_SIZE;
            int mapPixelHeight = ACTIVE_MAP_ROWS * TILE_SIZE;

            int wantW  = mapPixelWidth + 100 + PANEL_WIDTH;
            int wantH  = mapPixelHeight + 100;

            int monitorW = GetMonitorWidth(0);
            int monitorH = GetMonitorHeight(0);
            if (monitorW <= 0) monitorW = 1366;
            if (monitorH <= 0) monitorH = 768;
            int maxW = monitorW - 20;
            int maxH = monitorH - 40;
            if (maxW < 400) maxW = 400;
            if (maxH < 300) maxH = 300;

            int desiredW = wantW > maxW ? maxW : wantW;
            int desiredH = wantH > maxH ? maxH : wantH;
            if (desiredW <= 0) desiredW = initialW;
            if (desiredH <= 0) desiredH = initialH;

            int minimalWidthForRightPanel = 900;
            if (GetScreenWidth() < minimalWidthForRightPanel || desiredW < minimalWidthForRightPanel) {
                panelAtBottom = true;
            } else {
                panelAtBottom = false;
            }

            int screenW = GetScreenWidth();
            int screenH = GetScreenHeight();

            float viewX = (float)mapOriginX;
            float viewY = (float)mapOriginY;
            float viewW, viewH;
            int panelX = 0, panelY = 0, panelW = 0, panelH = 0;

            if (panelVisible) {
                if (panelAtBottom) {
                    panelH = PANEL_HEIGHT_BOTTOM;
                    panelW = screenW;
                    panelX = 0;
                    panelY = screenH - panelH;
                    viewW = (float)screenW - 20.0f;
                    viewH = (float)(screenH - panelH - (int)viewY - 10);
                    viewX = 10.0f;
                } else {
                    panelW = PANEL_WIDTH;
                    panelH = screenH;
                    panelX = screenW - panelW;
                    panelY = 0;
                    viewW = (float)(screenW - panelW - (int)viewX - 10);
                    viewH = (float)(screenH - (int)viewY - 10);
                }
            } else {
                viewX = 10.0f;
                viewY = 10.0f;
                viewW = (float)(screenW - 20);
                viewH = (float)(screenH - 20);
            }

            if (viewW < 32) viewW = 32;
            if (viewH < 32) viewH = 32;

            camera.zoom = 1.0f;
            camera.offset = (Vector2){ viewX + viewW * 0.5f, viewY + viewH * 0.5f };
            camera.rotation = 0.0f;

            Vector2 inputDir = {0.0f, 0.0f};
            if (IsKeyDown(KEY_RIGHT)) inputDir.x += 1.0f;
            if (IsKeyDown(KEY_LEFT))  inputDir.x -= 1.0f;
            if (IsKeyDown(KEY_DOWN))  inputDir.y += 1.0f;
            if (IsKeyDown(KEY_UP))    inputDir.y -= 1.0f;

            if (!collided && !rules.levelFailed && !rules.paused) {
                if (inputDir.x != 0.0f || inputDir.y != 0.0f) {
                    float len = sqrtf(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
                    if (len > 0.0f) {
                        player.direction.x = inputDir.x / len;
                        player.direction.y = inputDir.y / len;
                    }

                    Vector2 nextPos = {
                        player.position.x + player.direction.x * player.speed * dt,
                        player.position.y + player.direction.y * player.speed * dt
                    };

                    if (rules.level < 5) {
                        if (IsCarOnRoadSurface(nextPos))
                            player.position = nextPos;
                    } else {
                        player.position = nextPos;
                    }
                }
            }

            if (!collided && !rules.levelFailed && !rules.paused) {
                for (int i = 0; i < aiCount; ++i) {
                    UpdateAICar(&aiCars[i], dt);
                }
            }

            if (!rules.paused && !collided && !rules.levelFailed) {
                UpdateOrders(&rules, player.position);
            }

            if (!rules.paused && !collided && !rules.levelFailed) {
                UpdateOffRoad(&rules, player.position, dt);
            }

            if (!collided && !rules.levelFailed && !rules.paused) {
                Rectangle playerRectWorld = { player.position.x - 12.0f, player.position.y - 8.0f, 24.0f, 16.0f };
                for (int i = 0; i < aiCount; ++i) {
                    Rectangle aiRectWorld = { aiCars[i].position.x - 12.0f, aiCars[i].position.y - 8.0f, 24.0f, 16.0f };
                    if (CheckCollisionRecs(playerRectWorld, aiRectWorld)) {
                        collided = true;
                        collideTimer = 0.0f;
                        break;
                    }
                }
            }

            if (!rules.paused && !collided && !rules.levelFailed) {
                levelTimeRemaining -= dt;
                if (levelTimeRemaining <= 0.0f) {
                    rules.levelFailed = true;
                    levelFailTimer = 0.0f;
                }
            }

            {
                float mapLeft   = (float)mapOriginX;
                float mapTop    = (float)mapOriginY;
                float mapRight  = (float)(mapOriginX + ACTIVE_MAP_COLS * TILE_SIZE);
                float mapBottom = (float)(mapOriginY + ACTIVE_MAP_ROWS * TILE_SIZE);

                // Previously there was a "loseOnExit" toggle that only caused a fail
                // when the player left the map bounds. We removed the toggle and now
                // enforce failure when the player either leaves the map bounds OR
                // when the car is no longer on a road surface (i.e. off-road).
                if (!rules.paused && !collided && !rules.levelFailed) {
                    bool outsideMap = (player.position.x < mapLeft || player.position.x > mapRight ||
                                       player.position.y < mapTop  || player.position.y > mapBottom);

                    // Check whether the player's position is on the road surface.
                    bool onRoad = IsCarOnRoadSurface(player.position);

                    if (outsideMap || !onRoad) {
                        // accumulate time spent outside/ off-road
                        exitOffRoadTimer += dt;
                        // Update rules.offRoadTimer so the existing UI/logic that inspects
                        // rules.offRoadTimer continues to work (e.g., display). Cap it.
                        rules.offRoadTimer = exitOffRoadTimer;

                        if (exitOffRoadTimer >= EXIT_OFFROAD_TOLERANCE) {
                            rules.levelFailed = true;
                            levelFailTimer = 0.0f;
                        }
                    } else {
                        // back on road and inside bounds: reset the timer
                        exitOffRoadTimer = 0.0f;
                        rules.offRoadTimer = 0.0f;
                    }
                }
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && panelVisible && !collided && !rules.levelFailed) {
                Vector2 m = GetMousePosition();
                int listStartY = 60;
                int itemHeight = 50;

                int totalPages = (rules.orderCount + ORDERS_PER_PAGE - 1) / ORDERS_PER_PAGE;
                if (totalPages < 1) totalPages = 1;
                if (ordersPage < 0) ordersPage = 0;
                if (ordersPage >= totalPages) ordersPage = totalPages - 1;

                Rectangle prevBtn = {0}, nextBtn = {0};
                Rectangle listArea = {0};

                if (panelAtBottom) {
                    float lx = panelX + 20;
                    float lw = panelW - 40;
                    listArea = (Rectangle){ lx, (float)(panelY + listStartY), lw, (float)(itemHeight * ORDERS_PER_PAGE) };

                    float btnY = listArea.y + listArea.height + 8;
                    prevBtn = (Rectangle){ panelX + 20, (int)btnY, 40, 28 };
                    nextBtn = (Rectangle){ panelX + panelW - 60, (int)btnY, 40, 28 };
                } else {
                    float lx = panelX + 20;
                    float lw = panelW - 40;
                    listArea = (Rectangle){ lx, (float)listStartY, lw, (float)(itemHeight * ORDERS_PER_PAGE) };

                    float btnY = listArea.y + listArea.height + 8;
                    prevBtn = (Rectangle){ panelX + 20, (int)btnY, 40, 28 };
                    nextBtn = (Rectangle){ panelX + 70, (int)btnY, 40, 28 };
                }

                if (CheckCollisionPointRec(m, prevBtn)) {
                    if (ordersPage > 0) ordersPage--;
                } else if (CheckCollisionPointRec(m, nextBtn)) {
                    if (ordersPage < totalPages - 1) ordersPage++;
                } else if (CheckCollisionPointRec(m, listArea)) {
                    int localIdx = (int)((m.y - listArea.y) / itemHeight);
                    if (localIdx >= 0 && localIdx < ORDERS_PER_PAGE) {
                        int globalIdx = ordersPage * ORDERS_PER_PAGE + localIdx;
                        if (globalIdx >= 0 && globalIdx < rules.orderCount) {
                            Order *o = &rules.orders[globalIdx];
                            if (o->active) {
                                AcceptOrder(&rules, o->id);
                                float dx = player.position.x - o->shopPos.x;
                                float dy = player.position.y - o->shopPos.y;
                                float dist = sqrtf(dx*dx + dy*dy);
                                #ifdef PICKUP_RADIOUS
                                    if (dist < PICKUP_RADIOUS) {
                                        o->status = ORDER_PICKED_UP;
                                    }
                                #else
                                    if (dist < PICKUP_RADIUS) {
                                        o->status = ORDER_PICKED_UP;
                                    }
                                #endif
                            }
                        }
                    }
                }
            }

            if (rules.ordersCompleted >= rules.orderCount) {
                int nextLevel = rules.level + 1;

                int newRows = baseRows + (nextLevel - 1);
                int newCols = baseCols + (nextLevel - 1);
                if (newRows > GRID_H) newRows = GRID_H;
                if (newCols > GRID_W) newCols = GRID_W;

                ACTIVE_MAP_ROWS = newRows;
                ACTIVE_MAP_COLS = newCols;

                mapstatic_rules(ACTIVE_MAP_ROWS, ACTIVE_MAP_COLS);
                ClearGridOutsideActive(ACTIVE_MAP_ROWS, ACTIVE_MAP_COLS);

                player.position = (Vector2){50 + TILE_SIZE/2, 50 + TILE_SIZE/2};

                aiCount = 3 + (nextLevel - 1) * 2;
                if (aiCount > MAX_AI) aiCount = MAX_AI;

                {
                    bool used[GRID_H][GRID_W];
                    for (int rr = 0; rr < GRID_H; ++rr)
                        for (int cc = 0; cc < GRID_W; ++cc)
                            used[rr][cc] = false;

                    int pcol = (int)((player.position.x - mapOriginX) / TILE_SIZE);
                    int prow = (int)((player.position.y - mapOriginY) / TILE_SIZE);
                    if (prow < 0) prow = 0;
                    if (pcol < 0) pcol = 0;
                    if (prow >= ACTIVE_MAP_ROWS) prow = ACTIVE_MAP_ROWS - 1;
                    if (pcol >= ACTIVE_MAP_COLS) pcol = ACTIVE_MAP_COLS - 1;
                    used[prow][pcol] = true;

                    for (int i = 0; i < aiCount; ++i) {
                        bool placed = false;
                        int attempts = 0;
                        while (!placed && attempts < 2000) {
                            attempts++;
                            int rr = rand() % ACTIVE_MAP_ROWS;
                            int cc = rand() % ACTIVE_MAP_COLS;
                            if (used[rr][cc]) continue;

                            float px = (float)(mapOriginX + cc * TILE_SIZE + TILE_SIZE/2);
                            float py = (float)(mapOriginY + rr * TILE_SIZE + TILE_SIZE/2);
                            Vector2 pos = { px, py };

                            if (!IsCarOnRoadSurface(pos)) continue;

                            aiCars[i] = (AI_Car){{px, py}, 150.0f, {1.0f, 0.0f}, 0, 0};
                            used[rr][cc] = true;
                            placed = true;
                        }

                        if (!placed) {
                            for (int rr = 0; rr < ACTIVE_MAP_ROWS && !placed; ++rr) {
                                for (int cc = 0; cc < ACTIVE_MAP_COLS && !placed; ++cc) {
                                    if (used[rr][cc]) continue;
                                    float px = (float)(mapOriginX + cc * TILE_SIZE + TILE_SIZE/2);
                                    float py = (float)(mapOriginY + rr * TILE_SIZE + TILE_SIZE/2);
                                    Vector2 pos = { px, py };
                                    if (!IsCarOnRoadSurface(pos)) continue;
                                    aiCars[i] = (AI_Car){{px, py}, 150.0f, {1.0f, 0.0f}, 0, 0};
                                    used[rr][cc] = true;
                                    placed = true;
                                }
                            }
                        }

                        if (!placed) {
                            float px = player.position.x + TILE_SIZE;
                            float py = player.position.y + TILE_SIZE;
                            aiCars[i] = (AI_Car){{px, py}, 150.0f, {1.0f, 0.0f}, 0, 0};
                        }
                    }
                }

                StartLevel(&rules, nextLevel);
                SaveLevelToFile(nextLevel);
                levelTimeRemaining = 120.0f;
            }

            camera.target = player.position;

            float halfViewW_world = (viewW * 0.5f) / camera.zoom;
            float halfViewH_world = (viewH * 0.5f) / camera.zoom;

            float mapLeft   = (float)mapOriginX;
            float mapTop    = (float)mapOriginY;
            float mapRight  = (float)(mapOriginX + ACTIVE_MAP_COLS * TILE_SIZE);
            float mapBottom = (float)(mapOriginY + ACTIVE_MAP_ROWS * TILE_SIZE);

            if (mapRight - mapLeft <= 2.0f * halfViewW_world) {
                camera.target.x = (mapLeft + mapRight) * 0.5f;
            } else {
                if (camera.target.x < mapLeft + halfViewW_world) camera.target.x = mapLeft + halfViewW_world;
                if (camera.target.x > mapRight - halfViewW_world) camera.target.x = mapRight - halfViewW_world;
            }

            if (mapBottom - mapTop <= 2.0f * halfViewH_world) {
                camera.target.y = (mapTop + mapBottom) * 0.5f;
            } else {
                if (camera.target.y < mapTop + halfViewH_world) camera.target.y = mapTop + halfViewH_world;
                if (camera.target.y > mapBottom - halfViewH_world) camera.target.y = mapBottom - halfViewH_world;
            }

            for (int rr = 0; rr < ACTIVE_MAP_ROWS; ++rr)
                for (int cc = 0; cc < ACTIVE_MAP_COLS; ++cc)
                    pathGrid[rr][cc] = false;

            Order *active = GetActiveOrder(&rules);

            if (active) {
                Vector2 target = active->shopPos;
                if (active->status == ORDER_PICKED_UP) target = active->deliveryPos;

                int pathLen = 0;

                /* L-shaped path using rules_path1 (path_r -> path[][] -> pathTiles) */
                {
                    int sr, sc, tr, tc;
                    PosToTileCoords(player.position, mapOriginX, mapOriginY, &sr, &sc);
                    PosToTileCoords(target, mapOriginX, mapOriginY, &tr, &tc);

                    path_r(tr, tc, sr, sc);

                    int idx = 0;
                    for (int r = 0; r < ACTIVE_MAP_ROWS && idx < MAX_PATH; ++r) {
                        for (int c = 0; c < ACTIVE_MAP_COLS && idx < MAX_PATH; ++c) {
                            if (path[r][c]) {
                                pathTiles[idx][0] = r;
                                pathTiles[idx][1] = c;
                                idx++;
                            }
                        }
                    }
                    pathLen = idx;
                }



                for (int i = 0; i < pathLen; ++i) {
                    int pr = pathTiles[i][0];
                    int pc = pathTiles[i][1];
                    if (pr >= 0 && pr < ACTIVE_MAP_ROWS && pc >= 0 && pc < ACTIVE_MAP_COLS)
                        pathGrid[pr][pc] = true;
                }
            }

            BeginDrawing();
            ClearBackground(GREEN);

            BeginMode2D(camera);
            for (int r = 0; r < ACTIVE_MAP_ROWS; ++r) {
                for (int c = 0; c < ACTIVE_MAP_COLS; ++c) {
                    int roadMode = pathGrid[r][c] ? 1 : 0;
                    DrawTile(mapOriginX + c * TILE_SIZE, mapOriginY + r * TILE_SIZE, grid[r][c], roadMode, 0);
                }
            }

            for (int i = 0; i < rules.orderCount; ++i) {
                Order *o = &rules.orders[i];
                if (!o->active) continue;
                DrawCircleV(o->shopPos, 6, BLUE);
                DrawCircleV(o->deliveryPos, 6, RED);
            }

            if (active) {
                DrawLineV(player.position, active->shopPos, YELLOW);
                if (active->status == ORDER_PICKED_UP)
                    DrawLineV(active->shopPos, active->deliveryPos, ORANGE);
            }

            {
                Rectangle srcMain = { 0.0f, 0.0f, (float)mainCarTexture.width, (float)mainCarTexture.height };
                Rectangle dstMain = { player.position.x, player.position.y, 24.0f, 16.0f };
                Vector2 originMain = { dstMain.width/2.0f, dstMain.height/2.0f };
                float angleMain = atan2f(player.direction.y, player.direction.x) * (180.0f / M_PI);
                DrawTexturePro(mainCarTexture, srcMain, dstMain, originMain, angleMain, WHITE);
            }

            for (int i = 0; i < aiCount; ++i) {
                Rectangle srcAI = { 0.0f, 0.0f, (float)aiCarTexture.width, (float)aiCarTexture.height };
                Rectangle dstAI = { aiCars[i].position.x, aiCars[i].position.y, 24.0f, 16.0f };
                Vector2 originAI = { dstAI.width/2.0f, dstAI.height/2.0f };
                float angleAI = atan2f(aiCars[i].direction.y, aiCars[i].direction.x) * (180.0f / M_PI);
                DrawTexturePro(aiCarTexture, srcAI, dstAI, originAI, angleAI, WHITE);
            }

            EndMode2D();

            int timeMin = (int)(levelTimeRemaining / 60.0f);
            int timeSec = (int)(levelTimeRemaining) % 60;
            char timeBuf[32];
            sprintf(timeBuf, "Time: %d:%02d", timeMin, timeSec);
            DrawText(timeBuf, 12, 12, 18, BLACK);

            if (rules.level >= 5 && rules.offRoadTimer > 0.0f) {
                char buf[64];
                float shown = rules.offRoadTimer;
                if (shown > 2.0f) shown = 2.0f;
                sprintf(buf, "OFF ROAD: %.1f / 2.0", shown);
                DrawText(buf, 12, 36, 16, RED);
            }

            if (collided || rules.levelFailed) {
                if (collided) {
                    collideTimer += dt;
                } else {
                    levelFailTimer += dt;
                }

                DrawRectangle(0, 0, screenW, screenH, Fade(BLACK, 0.6f));

                const char *msg;
                if (collided) msg = "LEVEL FAILURE - COLLISION";
                else {
                    if (rules.offRoadTimer >= 2.0f) {
                        msg = "LEVEL FAILURE - OFF ROAD";
                    } else if (levelTimeRemaining <= 0.0f) {
                        msg = "LEVEL FAILURE - TIME UP";
                    } else {
                        msg = "LEVEL FAILURE";
                    }
                }

                int fontSize = 28;
                int textW = MeasureText(msg, fontSize);
                DrawText(msg, screenW/2 - textW/2, screenH/2 - fontSize/2, fontSize, RED);

                const char *sub = "Press ENTER to return to level menu";
                int subSz = 18;
                int subW = MeasureText(sub, subSz);
                DrawText(sub, screenW/2 - subW/2, screenH/2 + 30, subSz, RAYWHITE);

                if (IsKeyPressed(KEY_ENTER) || (collided && collideTimer > COLLIDE_SHOW_TIME) || (rules.levelFailed && levelFailTimer > COLLIDE_SHOW_TIME)) {
                    returnToMenu = true;
                }
            }

            if (panelVisible) {
                if (panelAtBottom) {
                    DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.6f));
                    DrawLine(panelX, panelY, panelX + panelW, panelY, DARKGRAY);
                    DrawText(TextFormat("ORDERS - Level %d", rules.level), panelX + 10, panelY + 6, 20, YELLOW);
                    DrawText(TextFormat("Map: %dx%d tiles", ACTIVE_MAP_ROWS, ACTIVE_MAP_COLS), panelX + 10, panelY + 32, 16, LIGHTGRAY);
                } else {
                    DrawRectangle(panelX, panelY, panelW, panelH, Fade(BLACK, 0.6f));
                    DrawLine(panelX, 0, panelX, panelH, DARKGRAY);
                    DrawText(TextFormat("ORDERS - Level %d", rules.level), panelX + 20, 20, 22, YELLOW);
                    DrawText(TextFormat("Map: %dx%d tiles", ACTIVE_MAP_ROWS, ACTIVE_MAP_COLS), panelX + 20, 46, 18, LIGHTGRAY);
                }

                if (!collided && !rules.levelFailed && DrawMenuButtonAndCheckClick(panelX, panelY, panelW, panelH, panelAtBottom)) {
                    returnToMenu = true;
                }

                int listStartY = panelAtBottom ? (panelY + 60) : 60;
                int listBaseX = panelAtBottom ? (panelX + 20) : (panelX + 20);
                int itemH = 50;

                int totalPages = (rules.orderCount + ORDERS_PER_PAGE - 1) / ORDERS_PER_PAGE;
                if (totalPages < 1) totalPages = 1;
                if (ordersPage < 0) ordersPage = 0;
                if (ordersPage >= totalPages) ordersPage = totalPages - 1;

                int start = ordersPage * ORDERS_PER_PAGE;
                int end = start + ORDERS_PER_PAGE;
                if (end > rules.orderCount) end = rules.orderCount;

                Vector2 mouse = GetMousePosition();

                for (int i = start; i < end; i++) {
                    int localI = i - start;
                    Order *o = &rules.orders[i];
                    Color textColor = WHITE;
                    if (!o->active) textColor = DARKGRAY;
                    else if (rules.activeOrderId == o->id) textColor = YELLOW;

                    Rectangle itemRect = { (float)listBaseX, (float)(listStartY + localI*itemH), (float)(panelW - 40), (float)(itemH-8) };
                    if (CheckCollisionPointRec(mouse, itemRect)) {
                        DrawRectangleRec(itemRect, Fade(GRAY, 0.08f));
                    }

                    DrawText(TextFormat("%d) Pay: %.2f", i + 1, o->minReward), listBaseX + 4, listStartY + localI*itemH + 4, 16, textColor);

                    if (o->active) {
                        const char *statusText =
                            (o->status == ORDER_AVAILABLE) ? "Go to shop" :
                            (o->status == ORDER_PICKED_UP) ? "Deliver" :
                            (o->status == ORDER_DELIVERED) ? "Done" :
                            "Failed";
                        DrawText(statusText, listBaseX + 4, listStartY + localI*itemH + 22, 12, GRAY);
                    } else {
                        DrawText("Inactive", listBaseX + 4, listStartY + localI*itemH + 22, 12, DARKGRAY);
                    }

                    if (o->status == ORDER_PICKED_UP) {
                        DrawText("Picked up", listBaseX + 120, listStartY + localI*itemH + 6, 14, ORANGE);
                    }
                }

                float lx = panelAtBottom ? (panelX + 20) : (panelX + 20);
                float lw = panelW - 40;
                Rectangle listArea = (Rectangle){ lx, (float)listStartY, lw, (float)(itemH * ORDERS_PER_PAGE) };
                float btnY = listArea.y + listArea.height + 8;

                Rectangle prevBtn, nextBtn;
                if (panelAtBottom) {
                    prevBtn = (Rectangle){ panelX + 20, (int)btnY, 40, 28 };
                    nextBtn = (Rectangle){ panelX + panelW - 60, (int)btnY, 40, 28 };
                } else {
                    prevBtn = (Rectangle){ panelX + 20, (int)btnY, 40, 28 };
                    nextBtn = (Rectangle){ panelX + 70, (int)btnY, 40, 28 };
                }

                DrawRectangleRec(prevBtn, Fade(DARKGRAY, 0.25f));
                DrawText("<", (int)prevBtn.x + 12, (int)prevBtn.y + 4, 20, RAYWHITE);
                if (CheckCollisionPointRec(mouse, prevBtn)) DrawRectangleLines((int)prevBtn.x, (int)prevBtn.y, (int)prevBtn.width, (int)prevBtn.height, YELLOW);

                DrawRectangleRec(nextBtn, Fade(DARKGRAY, 0.25f));
                DrawText(">", (int)nextBtn.x + 12, (int)nextBtn.y + 4, 20, RAYWHITE);
                if (CheckCollisionPointRec(mouse, nextBtn)) DrawRectangleLines((int)nextBtn.x, (int)nextBtn.y, (int)nextBtn.width, (int)nextBtn.height, YELLOW);

                Rectangle pauseBtn;
                if (panelAtBottom) {
                    pauseBtn = (Rectangle){ panelX + panelW - 110, (int)(btnY + PAUSE_OFFSET_Y), 40, 28 };
                } else {
                    pauseBtn = (Rectangle){ panelX + 120, (int)(btnY + PAUSE_OFFSET_Y), 40, 28 };
                }

                DrawRectangleRec(pauseBtn, Fade(DARKGRAY, 0.25f));
                const char *pauseLabel = rules.paused ? "PLAY" : "PAUSE";
                int plw = MeasureText(pauseLabel, 18);
                DrawText(pauseLabel, (int)(pauseBtn.x + (pauseBtn.width - plw)/2), (int)(pauseBtn.y + 4), 18, RAYWHITE);
                if (CheckCollisionPointRec(mouse, pauseBtn)) DrawRectangleLines((int)pauseBtn.x, (int)pauseBtn.y, (int)pauseBtn.width, (int)pauseBtn.height, YELLOW);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, pauseBtn)) {
                    rules.paused = !rules.paused;
                }

                // The toggle for "Lose On Exit" was removed. There is no UI element anymore.

                char pageText[32];
                int displayPage = ordersPage + 1;
                sprintf(pageText, "Page %d/%d", displayPage, totalPages);
                int textW = MeasureText(pageText, 14);
                int midX = panelX + panelW / 2;
                DrawText(pageText, midX - textW/2, (int)btnY + 4, 14, LIGHTGRAY);
            } else {
                DrawText(TextFormat("Level %d  (TAB to toggle panel)", rules.level), 8, 8, 16, YELLOW);
            }

            int moneyBoxX = panelVisible ? (panelAtBottom ? (panelX + 10) : (panelX + 20)) : 8;
            int moneyBoxY = panelVisible ? (panelAtBottom ? (panelY + 10) : (screenH - 80)) : (screenH - 80);
            int moneyBoxW = panelVisible ? (panelAtBottom ? (panelW - 20) : (panelW - 40)) : 200;
            DrawRectangle(moneyBoxX, moneyBoxY, moneyBoxW, 60, Fade(DARKGREEN, 0.2f));
            DrawText(TextFormat("Money: %.2f", rules.playerMoney), moneyBoxX + 8, moneyBoxY + 12, 18, GREEN);

            DrawText("Controls:", moneyBoxX, moneyBoxY - 70, 14, LIGHTGRAY);
            DrawText("Arrows: move", moneyBoxX, moneyBoxY - 50, 12, LIGHTGRAY);
            DrawText("TAB: toggle panel", moneyBoxX, moneyBoxY - 34, 12, LIGHTGRAY);

            EndDrawing();

            if (returnToMenu) break;
        }

        if (WindowShouldClose()) break;
        if (returnToMenu) continue;

        SaveLevelToFile(rules.level);
        break;
    }

    UnloadTexture(mainCarTexture);
    UnloadTexture(aiCarTexture);

    CloseWindow();
    return 0;
}
