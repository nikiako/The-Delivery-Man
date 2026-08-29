/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */

#ifndef RULES_H
#define RULES_H

#include "raylib.h"
#include <stdbool.h>

/* ============================================================
 * Constants and limits
 * ============================================================
 */

#define MAX_ORDERS 64

#define DELIVERY_RADIOUS 30.0f
#define PICKUP_RADIOUS   30.0f


/* ============================================================
 * OrderStatus
 * ============================================================
 * Summary:
 *   Enumeration describing the lifecycle state of an order.
 *
 * Description:
 *   Defines all possible states an order can be in during
 *   gameplay, from availability to completion or failure.
 */
typedef enum {
    ORDER_AVAILABLE = 0,
    ORDER_PICKED_UP,
    ORDER_DELIVERED,
    ORDER_FAILED
} OrderStatus;


/* ============================================================
 * Order
 * ============================================================
 * Summary:
 *   Data structure representing a single delivery order.
 *
 * Description:
 *   Stores all spatial, economic and state-related information
 *   required to manage a delivery order during gameplay.
 */
typedef struct {
    int id;
    Vector2 shopPos;
    Vector2 deliveryPos;
    float baseFee;
    float heightMultiplier;
    float minReward;
    float tip;

    OrderStatus status;
    bool active;
} Order;


/* ============================================================
 * GameRules
 * ============================================================
 * Summary:
 *   Central structure holding the game rules state.
 *
 * Description:
 *   Contains all rule-related runtime data such as orders,
 *   player progress, money, level status and off-road logic.
 */
typedef struct {
    Order orders[MAX_ORDERS];
    int orderCount;
    int activeOrderId;
    float playerMoney;
    int level;
    int ordersCompleted;
    bool paused;

    /* Off-road mechanics */
    bool offRoad;
    float offRoadTimer;
    bool levelFailed;
} GameRules;


/* ============================================================
 * InitGameRules
 * ============================================================
 * Summary:
 *   Initializes the game rules system.
 *
 * Description:
 *   Resets all rule-related values to their default state,
 *   preparing the GameRules structure for a new game session.
 *
 * Parameters:
 *   rules - Pointer to the GameRules structure to initialize.
 *
 * Return value:
 *   None.
 */
void InitGameRules(GameRules *rules);


/* ============================================================
 * StartLevel
 * ============================================================
 * Summary:
 *   Initializes a new level.
 *
 * Description:
 *   Sets up level-dependent parameters, generates orders and
 *   resets all per-level state variables.
 *
 * Parameters:
 *   rules - Pointer to the GameRules structure.
 *   level - Level number to start.
 *
 * Return value:
 *   None.
 */
void StartLevel(GameRules *rules, int level);


/* ============================================================
 * GenerateOrders
 * ============================================================
 * Summary:
 *   Generates delivery orders for the current level.
 *
 * Description:
 *   Creates and initializes a specified number of delivery
 *   orders with randomized locations and rewards.
 *
 * Parameters:
 *   rules - Pointer to the GameRules structure.
 *   count - Number of orders to generate.
 *
 * Return value:
 *   None.
 */
void GenerateOrders(GameRules *rules, int count);


/* ============================================================
 * UpdateOrders
 * ============================================================
 * Summary:
 *   Updates the state of the active order.
 *
 * Description:
 *   Checks player proximity to pickup and delivery locations
 *   and updates order status accordingly.
 *
 * Parameters:
 *   rules     - Pointer to the GameRules structure.
 *   playerPos - Current player world position.
 *
 * Return value:
 *   None.
 */
void UpdateOrders(GameRules *rules, Vector2 playerPos);


/* ============================================================
 * UpdateOffRoad
 * ============================================================
 * Summary:
 *   Updates off-road gameplay mechanics.
 *
 * Description:
 *   Tracks whether the player is driving off-road and applies
 *   penalties or level failure when the allowed time is exceeded.
 *
 * Parameters:
 *   rules     - Pointer to the GameRules structure.
 *   playerPos - Current player world position.
 *   dt        - Delta time since last update (seconds).
 *
 * Return value:
 *   None.
 */
void UpdateOffRoad(GameRules *rules, Vector2 playerPos, float dt);


/* ============================================================
 * AcceptOrder
 * ============================================================
 * Summary:
 *   Accepts an available delivery order.
 *
 * Description:
 *   Sets the specified order as the currently active order,
 *   if no other order is active.
 *
 * Parameters:
 *   rules   - Pointer to the GameRules structure.
 *   orderId - Identifier of the order to accept.
 *
 * Return value:
 *   None.
 */
void AcceptOrder(GameRules *rules, int orderId);


/* ============================================================
 * GetActiveOrder
 * ============================================================
 * Summary:
 *   Returns the currently active order.
 *
 * Description:
 *   Provides direct access to the order that is currently
 *   being handled by the player.
 *
 * Parameters:
 *   rules - Pointer to the GameRules structure.
 *
 * Return value:
 *   Pointer to the active Order, or NULL if no order is active.
 */
Order* GetActiveOrder(GameRules *rules);


/* ============================================================
 * GetOrderById
 * ============================================================
 * Summary:
 *   Retrieves an order by its identifier.
 *
 * Description:
 *   Returns a pointer to the specified order if it exists
 *   and is within valid bounds.
 *
 * Parameters:
 *   rules - Pointer to the GameRules structure.
 *   id    - Order identifier.
 *
 * Return value:
 *   Pointer to the Order if found, otherwise NULL.
 */
Order* GetOrderById(GameRules *rules, int id);


/* ============================================================
 * CalculateOrderReward
 * ============================================================
 * Summary:
 *   Calculates the base reward for an order.
 *
 * Description:
 *   Computes the minimum reward value based on the base fee
 *   and a height multiplier.
 *
 * Parameters:
 *   baseFee          - Base payment for the order.
 *   heightMultiplier - Multiplier applied to the base fee.
 *
 * Return value:
 *   The calculated reward value.
 */
float CalculateOrderReward(float baseFee, float heightMultiplier);

/* ============================================================
 * Internal helper functions (documentation only)
 * ============================================================
 *
 * The following functions are implemented as static functions
 * in the corresponding .c file.
 *
 * They are NOT part of the public API and must not be called
 * from other modules.
 *
 * This section exists solely for documentation and clarity.
 */


/*
 * Function: DistanceVec
 * --------------------------------
 * Computes the Euclidean distance between two vectors.
 *
 * Parameters:
 *   a - First vector.
 *   b - Second vector.
 *
 * Returns:
 *   The distance between vectors a and b.
 */


/*
 * Function: TileHasHouses
 * --------------------------------
 * Checks whether a tile type can contain houses.
 *
 * Parameters:
 *   t - Tile type to be checked.
 *
 * Returns:
 *   Non-zero if the tile can contain houses,
 *   zero otherwise.
 */


/*
 * Function: FindRandomHouseTile
 * --------------------------------
 * Finds a random tile that contains houses.
 *
 * Parameters:
 *   out_r - Output pointer for tile row.
 *   out_c - Output pointer for tile column.
 *
 * Returns:
 *   1 on success, 0 if no suitable tile is found.
 */


/*
 * Function: GetHousePositionInTile
 * --------------------------------
 * Computes a house position inside a specific tile.
 *
 * Parameters:
 *   row        - Tile row index.
 *   col        - Tile column index.
 *   houseIndex - Index of the house within the tile.
 *
 * Returns:
 *   World-space position of the house.
 */


/*
 * Function: _FindDifferentHouseLocation
 * --------------------------------
 * Finds a house location different from a given position.
 *
 * Parameters:
 *   avoidPos - Position to avoid.
 *   out_r    - Output tile row.
 *   out_c    - Output tile column.
 *   outPos   - Output world position.
 *
 * Returns:
 *   1 on success, 0 on failure.
 */


/*
 * Function: IsTileRoad
 * --------------------------------
 * Checks whether a tile represents a road.
 *
 * Parameters:
 *   t - Tile type to check.
 *
 * Returns:
 *   Non-zero if the tile is a road, zero otherwise.
 */


/*
 * Function: PosToTileIndex
 * --------------------------------
 * Converts a world position to a tile index.
 *
 * Parameters:
 *   p     - World position.
 *   out_r - Output tile row.
 *   out_c - Output tile column.
 *
 * Returns:
 *   Linear tile index.
 */


/*
 * Function: IsPlayerOnRoad
 * --------------------------------
 * Checks whether the player is currently on a road tile.
 *
 * Parameters:
 *   playerPos - Player world position.
 *
 * Returns:
 *   true if the player is on the road, false otherwise.
 */

#endif // RULES_H
