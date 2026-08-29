/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
#include "ai.h"
#include "road.h"     // grid, TILE_SIZE
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_AI_STATES 32

typedef struct {
    AI_Car *ai_ptr;
    bool inTileApproach;     // true while moving the pre-turn approach distance
    Vector2 approachTarget;  // world coords to move toward before applying plannedDirection
    Vector2 plannedDirection; // direction chosen at tile entry, applied after approach
    bool nudged;
    int nudgedTileX;
    int nudgedTileY;
} AIState;

static AIState g_states[MAX_AI_STATES] = {0};

static AIState *get_ai_state(AI_Car *ai) {
    for (int i = 0; i < MAX_AI_STATES; i++) if (g_states[i].ai_ptr == ai) return &g_states[i];
    for (int i = 0; i < MAX_AI_STATES; i++) {
        if (g_states[i].ai_ptr == NULL) {
            g_states[i].ai_ptr = ai;
            g_states[i].inTileApproach = false;
            g_states[i].approachTarget = (Vector2){0,0};
            g_states[i].plannedDirection = (Vector2){0,0};
            g_states[i].nudged = false;
            g_states[i].nudgedTileX = -9999;
            g_states[i].nudgedTileY = -9999;
            return &g_states[i];
        }
    }
    /* fallback */
    g_states[0].ai_ptr = ai;
    g_states[0].inTileApproach = false;
    g_states[0].approachTarget = (Vector2){0,0};
    g_states[0].plannedDirection = (Vector2){0,0};
    g_states[0].nudged = false;
    g_states[0].nudgedTileX = -9999;
    g_states[0].nudgedTileY = -9999;
    return &g_states[0];
}

/* Helper: opposite directions */
static bool IsOpposite(Vector2 a, Vector2 b) {
    return (a.x == -b.x && a.y == -b.y);
}


static Vector2 ChooseDirection(TileType t, Vector2 cur, bool allowReverse) {
    Vector2 cand[4];
    int n = 0;

    switch (t) {
        case HORIZONTAL:
            cand[n++] = (Vector2){1,0};
            cand[n++] = (Vector2){-1,0};
            break;
        case VERTICAL:
            cand[n++] = (Vector2){0,1};
            cand[n++] = (Vector2){0,-1};
            break;

        case TURN_UP_LEFT:
            cand[n++] = (Vector2){0,-1};
            cand[n++] = (Vector2){-1,0};
            break;
        case TURN_UP_RIGHT:
            cand[n++] = (Vector2){0,-1};
            cand[n++] = (Vector2){1,0};
            break;
        case TURN_DOWN_LEFT:
            cand[n++] = (Vector2){0,1};
            cand[n++] = (Vector2){-1,0};
            break;
        case TURN_DOWN_RIGHT:
            cand[n++] = (Vector2){0,1};
            cand[n++] = (Vector2){1,0};
            break;

        case T_UP:

            cand[n++] = (Vector2){-1,0};
            cand[n++] = (Vector2){ 1,0};
            cand[n++] = (Vector2){ 0,1};
            break;
        case T_DOWN:

            cand[n++] = (Vector2){-1,0};
            cand[n++] = (Vector2){ 1,0};
            cand[n++] = (Vector2){ 0,-1};
            break;
        case T_LEFT:

            cand[n++] = (Vector2){0,-1};
            cand[n++] = (Vector2){0, 1};
            cand[n++] = (Vector2){1, 0};
            break;

        case T_RIGHT:

            cand[n++] = (Vector2){0,-1};
            cand[n++] = (Vector2){0, 1};
            cand[n++] = (Vector2){-1,0};
            break;

        case CROSS:
            cand[n++] = (Vector2){1,0};
            cand[n++] = (Vector2){-1,0};
            cand[n++] = (Vector2){0,1};
            cand[n++] = (Vector2){0,-1};
            break;

        default:
            return cur;
    }

    Vector2 valid[4];
    int v = 0;
    for (int i = 0; i < n; i++)
        if (allowReverse || !IsOpposite(cand[i], cur))
            valid[v++] = cand[i];

    if (v == 0) return (Vector2){-cur.x, -cur.y};
    return valid[rand() % v];
}

/* Move into exit lane once: align perpendicular coordinate and small nudge
   modified so cars sit on the right-hand side of the road (right-hand traffic). */
static void MoveIntoExitLaneOnce(AI_Car *ai, AIState *state) {
    float epsilon = 4.0f;
    int mx = ai->tileX, my = ai->tileY;
    if (mx < 0 || mx >= GRID_W || my < 0 || my >= GRID_H) {
        mx = (int)((ai->position.x - 50) / TILE_SIZE);
        my = (int)((ai->position.y - 50) / TILE_SIZE);
    }
    int tileX = 50 + mx * TILE_SIZE;
    int tileY = 50 + my * TILE_SIZE;

    /* lane shift: how far from center place the car (keeps inside road band) */
    float laneShift = TILE_SIZE * 0.125f; /* 1/8 of tile -> inside the road band */
    float centerX = tileX + TILE_SIZE * 0.5f;
    float centerY = tileY + TILE_SIZE * 0.5f;

    if (ai->direction.x != 0) {
        /* moving horizontally: fix Y to the right-side lane.
           screen Y increases downwards, so:
             - moving right (x>0): right side = +Y (south)
             - moving left  (x<0): right side = -Y (north) */
        float sign = (ai->direction.x > 0) ? 1.0f : -1.0f;
        ai->position.y = centerY + sign * laneShift;
    } else if (ai->direction.y != 0) {
        /* moving vertically: fix X to the right-side lane.
           - moving down  (y>0): right side = west = -X
           - moving up    (y<0): right side = east = +X */
        float sign = (ai->direction.y > 0) ? -1.0f : 1.0f;
        ai->position.x = centerX + sign * laneShift;
    }

    ai->position.x += ai->direction.x * epsilon;
    ai->position.y += ai->direction.y * epsilon;

    int newMx = (int)((ai->position.x - 50) / TILE_SIZE);
    int newMy = (int)((ai->position.y - 50) / TILE_SIZE);
    if (newMx >= 0 && newMx < GRID_W) ai->tileX = newMx;
    if (newMy >= 0 && newMy < GRID_H) ai->tileY = newMy;

    state->nudged = true;
    state->nudgedTileX = mx;
    state->nudgedTileY = my;
}


/* Correct direction if next small step would be off-road (tries candidates and nudges once) */
static void CorrectDirectionIfAtEdge(AI_Car *ai) {
    AIState *state = get_ai_state(ai);

    int mx = (int)((ai->position.x - 50) / TILE_SIZE);
    int my = (int)((ai->position.y - 50) / TILE_SIZE);
    if (mx < 0 || mx >= GRID_W || my < 0 || my >= GRID_H) return;

    TileType t = grid[my][mx];

    Vector2 next = ai->position;
    next.x += ai->direction.x * ai->speed * 0.016f;
    next.y += ai->direction.y * ai->speed * 0.016f;

    if (IsCarOnRoadSurface(next)) return;

    Vector2 cand[4];
    int n = 0;
    switch(t) {
        case HORIZONTAL: cand[n++] = (Vector2){1,0}; cand[n++] = (Vector2){-1,0}; break;
        case VERTICAL:   cand[n++] = (Vector2){0,1}; cand[n++] = (Vector2){0,-1}; break;
        case TURN_UP_LEFT:    cand[n++] = (Vector2){0,-1}; cand[n++] = (Vector2){-1,0}; break;
        case TURN_UP_RIGHT:   cand[n++] = (Vector2){0,-1}; cand[n++] = (Vector2){1,0}; break;
        case TURN_DOWN_LEFT:  cand[n++] = (Vector2){0,1};  cand[n++] = (Vector2){-1,0}; break;
        case TURN_DOWN_RIGHT: cand[n++] = (Vector2){0,1};  cand[n++] = (Vector2){1,0}; break;
        case T_UP:    cand[n++] = (Vector2){-1,0}; cand[n++] = (Vector2){1,0}; cand[n++] = (Vector2){0,1}; break;
        case T_DOWN:  cand[n++] = (Vector2){-1,0}; cand[n++] = (Vector2){1,0}; cand[n++] = (Vector2){0,-1}; break;
        case T_LEFT:
            cand[n++] = (Vector2){0,-1};
            cand[n++] = (Vector2){0, 1};
            cand[n++] = (Vector2){1, 0};
            break;

        case T_RIGHT:
            cand[n++] = (Vector2){0,-1};
            cand[n++] = (Vector2){0, 1};
            cand[n++] = (Vector2){-1,0};
            break;
        case CROSS:   cand[n++] = (Vector2){1,0}; cand[n++] = (Vector2){-1,0}; cand[n++] = (Vector2){0,1}; cand[n++] = (Vector2){0,-1}; break;
        default: return;
    }

    for (int i = 0; i < n; i++) {
        next = ai->position;
        next.x += cand[i].x * ai->speed * 0.016f;
        next.y += cand[i].y * ai->speed * 0.016f;
        if (IsCarOnRoadSurface(next)) {
            ai->direction = cand[i];
            if (!(state->nudged && state->nudgedTileX == mx && state->nudgedTileY == my)) {
                MoveIntoExitLaneOnce(ai, state);
            }
            return;
        }
    }

    /* fallback: u-turn */
    ai->direction.x = -ai->direction.x;
    ai->direction.y = -ai->direction.y;
    if (!(state->nudged && state->nudgedTileX == mx && state->nudgedTileY == my)) {
        MoveIntoExitLaneOnce(ai, state);
    }
}

/* Compute approach target inside the tile: move forward along current direction by approachDistance,
   then clamp inside tile while keeping margin.  */
static Vector2 ComputeApproachTargetClamped(AI_Car *ai, float approachDistance) {
    int mx = (int)((ai->position.x - 50) / TILE_SIZE);
    int my = (int)((ai->position.y - 50) / TILE_SIZE);
    int tileX = 50 + mx * TILE_SIZE;
    int tileY = 50 + my * TILE_SIZE;

    Vector2 target = ai->position;
    target.x += ai->direction.x * approachDistance;
    target.y += ai->direction.y * approachDistance;

    float margin = 4.0f;

    /* lane shift: put car to the right side of the road */
    float laneShift = TILE_SIZE * 0.125f;
    float centerX = tileX + TILE_SIZE * 0.5f;
    float centerY = tileY + TILE_SIZE * 0.5f;

    if (ai->direction.x != 0) {
        /* horizontal travel: lock perpendicular (y) to right-side lane */
        float sign = (ai->direction.x > 0) ? 1.0f : -1.0f;
        target.y = centerY + sign * laneShift;
    } else if (ai->direction.y != 0) {
        /* vertical travel: lock perpendicular (x) to right-side lane
           moving down (y>0) -> right side = west (-x), moving up -> east (+x) */
        float sign = (ai->direction.y > 0) ? -1.0f : 1.0f;
        target.x = centerX + sign * laneShift;
    }


    if (target.x < tileX + margin) target.x = tileX + margin;
    if (target.x > tileX + TILE_SIZE - margin) target.x = tileX + TILE_SIZE - margin;
    if (target.y < tileY + margin) target.y = tileY + margin;
    if (target.y > tileY + TILE_SIZE - margin) target.y = tileY + TILE_SIZE - margin;

    return target;
}

/* Update AI per frame: choose plannedDirection at tile entry, move approachDistance, then apply it. */
void UpdateAICar(AI_Car *ai, float dt) {
    AIState *state = get_ai_state(ai);

    int mx = (int)((ai->position.x - 50) / TILE_SIZE);
    int my = (int)((ai->position.y - 50) / TILE_SIZE);
    if (mx < 0 || mx >= GRID_W || my < 0 || my >= GRID_H) return;

    TileType t = grid[my][mx];
    bool isTurnTile = (t == TURN_UP_LEFT || t == TURN_UP_RIGHT || t == TURN_DOWN_LEFT || t == TURN_DOWN_RIGHT);

    /* Clear nudged when leaving tile */
    if (state->nudged && (mx != state->nudgedTileX || my != state->nudgedTileY)) {
        state->nudged = false;
        state->nudgedTileX = -9999;
        state->nudgedTileY = -9999;
    }

    /* When entering a new tile: decide plannedDirection immediately and set approach target */
    if (mx != ai->tileX || my != ai->tileY) {
        ai->tileX = mx;
        ai->tileY = my;


        Vector2 planned = ChooseDirection(t, ai->direction, false);
        state->plannedDirection = planned;

        /* compute approach target: move a bit forward along current direction, clamped inside tile */
        float approachDistance = TILE_SIZE * 0.35f; // move ~35% of tile before applying the planned turn
        state->approachTarget = ComputeApproachTargetClamped(ai, approachDistance);
        state->inTileApproach = true;

    }

    /* If we are in the approach phase, move smoothly toward approachTarget along straight line */
    if (state->inTileApproach) {
        Vector2 target = state->approachTarget;
        float dx = target.x - ai->position.x;
        float dy = target.y - ai->position.y;
        float dist = sqrtf(dx*dx + dy*dy);

        if (dist <= 0.0001f) {
            /* reached: apply planned direction and nudge once */
            state->inTileApproach = false;
            ai->direction = state->plannedDirection;
            MoveIntoExitLaneOnce(ai, state);
        } else {
            float step = ai->speed * dt;
            if (dist <= step) {
                ai->position.x = target.x;
                ai->position.y = target.y;
                state->inTileApproach = false;
                ai->direction = state->plannedDirection;
                MoveIntoExitLaneOnce(ai, state);
            } else {
                float nx = dx / dist;
                float ny = dy / dist;
                ai->position.x += nx * step;
                ai->position.y += ny * step;
                /* set render direction to dominant axis for visuals */
                if (fabsf(nx) > fabsf(ny)) { ai->direction.x = (nx>0)?1:-1; ai->direction.y = 0; }
                else { ai->direction.x = 0; ai->direction.y = (ny>0)?1:-1; }
            }
        }

        return;
    }

    /* normal behavior: correct boundaries and move axis-aligned */
    CorrectDirectionIfAtEdge(ai);

    Vector2 next = ai->position;
    next.x += ai->direction.x * ai->speed * dt;
    next.y += ai->direction.y * ai->speed * dt;

    if (IsCarOnRoadSurface(next)) {
        ai->position = next;
    } else {
        /* try correction again */
        CorrectDirectionIfAtEdge(ai);
    }
}

/* Direction to rotation angle for rendering */
float DirectionToAngle(Vector2 d) {
    if (d.x == 1)  return 0.0f;
    if (d.x == -1) return 180.0f;
    if (d.y == 1)  return 90.0f;
    if (d.y == -1) return -90.0f;
    return 0.0f;
}
