/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */

// ===== file: menu.h =====
#ifndef MENU_H
#define MENU_H

/* ============================================================
 * ShowLevelSelection
 * ============================================================
 * Summary:
 *   Displays the level selection menu screen.
 *
 * Description:
 *   Renders an interactive level selection menu where the user
 *   can navigate through available levels using the keyboard
 *   or mouse and choose one of the unlocked levels.
 *
 * Parameters:
 *   unlockedLevel - Highest level currently unlocked by the player.
 *   initialW      - Initial window width used for layout centering.
 *   initialH      - Initial window height used for layout positioning.
 *
 * Return value:
 *   Returns the selected level number (1..MAX_LEVELS).
 *   Returns 0 if the user cancels the menu or chooses to quit.
 */
int ShowLevelSelection(int unlockedLevel, int initialW, int initialH);

#endif // MENU_H
