/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
// ===== file: save.c =====
#include "save.h"
#include <stdio.h>


int LoadSavedLevel(void) {
    FILE *f = fopen("savegame.txt", "r");
    if (!f) return 0;
    int lvl = 0;
    if (fscanf(f, "%d", &lvl) != 1) lvl = 0;
    fclose(f);
    if (lvl < 1) return 0;
    return lvl;
}


void SaveLevelToFile(int level) {
    int current = LoadSavedLevel();
    if (level <= current) return; // only save if it's greater
    FILE *f = fopen("savegame.txt", "w");
    if (!f) return;
    fprintf(f, "%d\n", level);
    fclose(f);
}
