/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
// rules.c
#include "rules.h"
#include "graphics.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h> // for debug printf

extern TileType grid[GRID_H][GRID_W];
extern int ACTIVE_MAP_ROWS;
extern int ACTIVE_MAP_COLS;

static float DistanceVec(Vector2 a, Vector2 b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx*dx + dy*dy);
}

static int TileHasHouses(TileType t) {
    return (t != EMPTY);
}

static int FindRandomHouseTile(int *out_r, int *out_c) {
    if (ACTIVE_MAP_ROWS <= 0 || ACTIVE_MAP_COLS <= 0) return 0;

    for (int tries = 0; tries < 200; ++tries) {
        int r = rand() % ACTIVE_MAP_ROWS;
        int c = rand() % ACTIVE_MAP_COLS;
        if (TileHasHouses(grid[r][c])) {
            *out_r = r; *out_c = c;
            return 1;
        }
    }
    for (int r = 0; r < ACTIVE_MAP_ROWS; ++r) {
        for (int c = 0; c < ACTIVE_MAP_COLS; ++c) {
            if (TileHasHouses(grid[r][c])) {
                *out_r = r; *out_c = c;
                return 1;
            }
        }
    }
    return 0;
}

static Vector2 GetHousePositionInTile(int row, int col, int houseIndex) {
    float tileX = 50.0f + col * (float)TILE_SIZE;
    float tileY = 50.0f + row * (float)TILE_SIZE;

    float leftX  = tileX + HOUSE_MARGIN;
    float rightX = tileX + TILE_SIZE - HOUSE_MARGIN - HOUSE_W;
    float topY   = tileY + HOUSE_TOP_MARGIN;
    float bottomY_upright  = tileY + TILE_SIZE - HOUSE_MARGIN - 2 * HOUSE_H;
    float bottomY_sideways = tileY + TILE_SIZE - HOUSE_MARGIN - HOUSE_W;

    Vector2 pos = { tileX + TILE_SIZE * 0.5f, tileY + TILE_SIZE * 0.5f };

    TileType t = grid[row][col];

    switch (t) {
        case VERTICAL: {
            float x = leftX;
            if (houseIndex == 0) {
                float y = topY;
                pos.x = x + 1.5f * (float)HOUSE_H;
                pos.y = y + 0.5f * (float)HOUSE_W;
            } else {
                float y = bottomY_sideways;
                pos.x = x + 1.5f * (float)HOUSE_H;
                pos.y = y + 0.5f * (float)HOUSE_W;
            }
            break;
        }

        case TURN_UP_LEFT:
        case TURN_DOWN_LEFT: {
            float x = leftX;
            if (houseIndex == 0) {
                float y = topY;
                pos.x = x + 0.5f * (float)HOUSE_W;
                pos.y = y + 1.5f * (float)HOUSE_H;
            } else {
                float y = bottomY_upright;
                pos.x = x + 0.5f * (float)HOUSE_W;
                pos.y = y + 1.5f * (float)HOUSE_H;
            }
            break;
        }

        case TURN_UP_RIGHT:
        case TURN_DOWN_RIGHT: {
            float x = rightX;
            if (houseIndex == 0) {
                float y = topY;
                pos.x = x + 0.5f * (float)HOUSE_W;
                pos.y = y + 1.5f * (float)HOUSE_H;
            } else {
                float y = bottomY_upright;
                pos.x = x + 0.5f * (float)HOUSE_W;
                pos.y = y + 1.5f * (float)HOUSE_H;
            }
            break;
        }

        case T_LEFT: {
            float x = leftX;
            if (houseIndex == 0) {
                float y = topY;
                pos.x = x + 1.5f * (float)HOUSE_H;
                pos.y = y + 0.5f * (float)HOUSE_W;
            } else {
                float y = bottomY_sideways;
                pos.x = x + 1.5f * (float)HOUSE_H;
                pos.y = y + 0.5f * (float)HOUSE_W;
            }
            break;
        }

        case T_RIGHT: {
            float x = tileX + TILE_SIZE - HOUSE_MARGIN - 2 * HOUSE_H;
            if (houseIndex == 0) {
                float y = topY;
                pos.x = x + 0.5f * (float)HOUSE_H;
                pos.y = y + 0.5f * (float)HOUSE_W;
            } else {
                float y = bottomY_sideways;
                pos.x = x + 0.5f * (float)HOUSE_H;
                pos.y = y + 0.5f * (float)HOUSE_W;
            }
            break;
        }

        case CROSS: {
            if (houseIndex == 0) {
                pos.x = leftX + 0.5f * (float)HOUSE_W;
                pos.y = topY + 1.5f * (float)HOUSE_H;
            } else {
                pos.x = rightX + 0.5f * (float)HOUSE_W;
                pos.y = topY + 1.5f * (float)HOUSE_H;
            }
            break;
        }

        case HORIZONTAL:
        default: {
            if (houseIndex == 0) {
                pos.x = leftX + 0.5f * (float)HOUSE_W;
                pos.y = topY + 1.5f * (float)HOUSE_H;
            } else {
                pos.x = rightX + 0.5f * (float)HOUSE_W;
                pos.y = topY + 1.5f * (float)HOUSE_H;
            }
            break;
        }
    }

    return pos;
}

void InitGameRules(GameRules *rules) {
    rules->orderCount = 0;
    rules->activeOrderId = -1;
    rules->playerMoney = 0.0f;
    rules->level = 1;
    rules->ordersCompleted = 0;
    rules->paused = false;
    rules->offRoad = false;
    rules->offRoadTimer = 0.0f;
    rules->levelFailed = false;
    for (int i = 0; i < MAX_ORDERS; ++i) rules->orders[i].active = false;
}

static int _FindDifferentHouseLocation(Vector2 avoidPos, int *out_r, int *out_c, Vector2 *outPos) {
    for (int attempts = 0; attempts < 500; ++attempts) {
        int r,c;
        if (!FindRandomHouseTile(&r,&c)) return 0;
        int hi = rand() % 2;
        Vector2 p = GetHousePositionInTile(r,c,hi);
        if (DistanceVec(p, avoidPos) > 30.0f) {
            *out_r = r; *out_c = c; *outPos = p;
            return 1;
        }
    }
    int r,c;
    if (!FindRandomHouseTile(&r,&c)) return 0;
    int hi = rand() % 2;
    *outPos = GetHousePositionInTile(r,c,hi);
    *out_r = r; *out_c = c;
    return 1;
}

void GenerateOrders(GameRules *rules, int count) {
    if (count > MAX_ORDERS) count = MAX_ORDERS;
    rules->orderCount = count;
    for (int i = 0; i < count; ++i) {
        Order *o = &rules->orders[i];
        o->id = i;

        int sr, sc;
        Vector2 shopPos;
        if (!FindRandomHouseTile(&sr, &sc)) {
            shopPos = (Vector2){
                50 + (rand() % ACTIVE_MAP_COLS) * TILE_SIZE + TILE_SIZE / 2,
                50 + (rand() % ACTIVE_MAP_ROWS) * TILE_SIZE + TILE_SIZE / 2
            };
        } else {
            int shouse = rand() % 2;
            shopPos = GetHousePositionInTile(sr, sc, shouse);
        }
        o->shopPos = shopPos;

        int dr, dc;
        Vector2 deliveryPos;
        _FindDifferentHouseLocation(o->shopPos, &dr, &dc, &deliveryPos);
        o->deliveryPos = deliveryPos;

        o->baseFee = 3.0f + (rand() % 5);
        o->heightMultiplier = 1.0f + (rand() % 3);
        o->minReward = CalculateOrderReward(o->baseFee, o->heightMultiplier);

        // tip only
        o->tip = (float)(rand() % 501) / 100.0f;


        o->status = ORDER_AVAILABLE;
        o->active = true;
    }
}

void UpdateOrders(GameRules *rules, Vector2 playerPos) {
    // update active order status
    Order *active = GetActiveOrder(rules);
    if (!active) return;

    if (active->status == ORDER_PICKED_UP) {
        // just check delivery proximity
        if (DistanceVec(playerPos, active->deliveryPos) < DELIVERY_RADIOUS) {
            active->status = ORDER_DELIVERED;
            active->active = false;
            rules->playerMoney += active->minReward + active->tip; // give tip on successful delivery
            rules->activeOrderId = -1;
            rules->ordersCompleted += 1;
            // debug
            // printf("[RULES] Order %d delivered\n", active->id);
            return;
        }
    } else if (active->status == ORDER_AVAILABLE) {
        // pickup when player reaches shop
        if (DistanceVec(playerPos, active->shopPos) < PICKUP_RADIOUS) {
            active->status = ORDER_PICKED_UP;
            // printf("[RULES] Order %d picked up\n", active->id);
        }
    }
}

void AcceptOrder(GameRules *rules, int orderId) {
    if (rules->activeOrderId != -1) return;
    Order *o = GetOrderById(rules, orderId);
    if (!o || !o->active) return;
    rules->activeOrderId = orderId;

}

Order* GetActiveOrder(GameRules *rules) {
    if (rules->activeOrderId < 0) return NULL;
    return &rules->orders[rules->activeOrderId];
}

Order* GetOrderById(GameRules *rules, int id) {
    if (id < 0 || id >= rules->orderCount) return NULL;
    return &rules->orders[id];
}

float CalculateOrderReward(float baseFee, float heightMultiplier) {
    return baseFee * heightMultiplier;
}

void StartLevel(GameRules *rules, int level) {
    if (level < 1) level = 1;
    rules->level = level;
    rules->ordersCompleted = 0;
    rules->activeOrderId = -1;

    int count = 3 + (level - 1);
    if (count > MAX_ORDERS) count = MAX_ORDERS;

    GenerateOrders(rules, count);

    for (int i = 0; i < rules->orderCount; ++i) {
        Order *o = &rules->orders[i];
        o->baseFee += (level - 1) * 0.5f;
        o->heightMultiplier += (level - 1) * 0.06f;
        o->minReward = CalculateOrderReward(o->baseFee, o->heightMultiplier);
    }

    // reset off-road state for new level
    rules->offRoad = false;
    rules->offRoadTimer = 0.0f;
    rules->levelFailed = false;
}

/* Utility: is tile a road surface? */
static inline int IsTileRoad(TileType t) {
    return (t != EMPTY);
}

/* Convert world pos to tile index */
static int PosToTileIndex(Vector2 p, int *out_r, int *out_c) {
    int c = (int)((p.x - 50.0f) / (float)TILE_SIZE);
    int r = (int)((p.y - 50.0f) / (float)TILE_SIZE);
    if (r < 0) r = 0;
    if (c < 0) c = 0;
    if (r >= ACTIVE_MAP_ROWS) r = ACTIVE_MAP_ROWS - 1;
    if (c >= ACTIVE_MAP_COLS) c = ACTIVE_MAP_COLS - 1;
    *out_r = r; *out_c = c;
    return r * ACTIVE_MAP_COLS + c;
}

/* Check whether a player position is on a road tile */
static bool IsPlayerOnRoad(Vector2 playerPos) {
    int r, c;
    PosToTileIndex(playerPos, &r, &c);
    if (r < 0 || c < 0 || r >= ACTIVE_MAP_ROWS || c >= ACTIVE_MAP_COLS) return false;
    return IsTileRoad(grid[r][c]);
}

/* Update off-road timer: from level 5 onward we allow off-road but fail after 2s */
void UpdateOffRoad(GameRules *rules, Vector2 playerPos, float dt) {
    if (rules->level < 5) {
        // levels 1-4: off-road prevention is handled in main (player can't move off road)
        rules->offRoad = false;
        rules->offRoadTimer = 0.0f;
        return;
    }

    bool onRoad = IsPlayerOnRoad(playerPos);
    rules->offRoad = !onRoad;

    if (!onRoad) {
        rules->offRoadTimer += dt;
        if (rules->offRoadTimer >= 2.0f) {
            rules->levelFailed = true;
            // debug
            // printf("[RULES] Level failed due to off-road (%.2f s)\n", rules->offRoadTimer);
        }
    } else {
        rules->offRoadTimer = 0.0f;
    }
}
