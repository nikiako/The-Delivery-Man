/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */

// ===== file: ui.h =====
#ifndef UI_H
#define UI_H

#include "raylib.h"
#include <stdbool.h>

/* ============================================================
 * DrawMenuButtonAndCheckClick
 * ============================================================
 * Summary:
 *   Draws a MENU button inside a UI panel and checks for mouse click.
 *
 * Description:
 *   Renders a small rectangular MENU button at the appropriate
 *   position inside a UI panel and detects whether the user has
 *   clicked on it using the left mouse button.
 *
 * Parameters:
 *   panelX         - X coordinate of the panel (top-left corner).
 *   panelY         - Y coordinate of the panel (top-left corner).
 *   panelW         - Width of the panel.
 *   panelH         - Height of the panel.
 *   panelAtBottom  - If true, the panel is located at the bottom
 *                    of the screen; otherwise it is assumed to be
 *                    positioned at the top/right area.
 *
 * Return value:
 *   Returns true if the MENU button was clicked during this frame,
 *   otherwise returns false.
 */
bool DrawMenuButtonAndCheckClick(int panelX, int panelY,int panelW, int panelH,bool panelAtBottom);

#endif // UI_H
