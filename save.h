/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */

// ===== file: save.h =====
#ifndef SAVE_H
#define SAVE_H

/* ============================================================
 * LoadSavedLevel
 * ============================================================
 * Summary:
 *   Loads the highest unlocked level from persistent storage.
 *
 * Description:
 *   Reads the saved level value from a local save file and
 *   validates it before returning it to the caller.
 *
 * Parameters:
 *   None.
 *
 * Return value:
 *   Returns the highest unlocked level number stored in the
 *   save file.
 *   Returns 0 if the save file does not exist, cannot be read,
 *   or contains invalid data.
 */
int LoadSavedLevel(void);


/* ============================================================
 * SaveLevelToFile
 * ============================================================
 * Summary:
 *   Saves the highest unlocked level to persistent storage.
 *
 * Description:
 *   Writes the given level number to the save file only if it
 *   is greater than the currently stored value.
 *
 * Parameters:
 *   level - The level number to be saved.
 *
 * Return value:
 *   None.
 */
void SaveLevelToFile(int level);

#endif // SAVE_H
