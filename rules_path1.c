/*
 * Copyright (c) 2026 IAKOVIDIS NIKOLAOS
 * Licensed under the GNU General Public License v3 (GPLv3)
 */
/* rules_path1.c */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rules_path1.h"


extern TileType grid[GRID_H][GRID_W];
extern int ACTIVE_MAP_ROWS;
extern int ACTIVE_MAP_COLS;


short path[GRID_H][GRID_W];


void mapstatic_rules(int r, int c)
{
    grid[0][0]       = TURN_DOWN_RIGHT;
    grid[0][c-1]     = TURN_DOWN_LEFT;
    grid[r-1][0]     = TURN_UP_RIGHT;
    grid[r-1][c-1]   = TURN_UP_LEFT;

    int kat;

    for(int i = 0; i < r-2; i++)
    {
        if(i < r-3)
            kat = rand() % 2;
        else
            kat = 0;

        if(kat == 0)
        {
            grid[i+1][0]   = T_LEFT;
            grid[i+1][c-1] = T_RIGHT;
        }
        else
        {
            grid[i+1][0]   = VERTICAL;
            grid[i+1][c-1] = VERTICAL;

            grid[i+2][0]   = T_LEFT;
            grid[i+2][c-1] = T_RIGHT;
            i++;
        }
    }

    for(int i = 0; i < c-2; i++)
    {
        if(i < c-3)
            kat = rand() % 2;
        else
            kat = 0;

        if(kat == 0)
        {
            grid[0][i+1]   = T_UP;
            grid[r-1][i+1] = T_DOWN;
        }
        else
        {
            grid[0][i+1]   = HORIZONTAL;
            grid[r-1][i+1] = HORIZONTAL;

            grid[0][i+2]   = T_UP;
            grid[r-1][i+2] = T_DOWN;
            i++;
        }
    }

    for(int i = 1; i < r-1; i++)
    {
        for(int j = 1; j < c-1; j++)
        {
            if(grid[i][0] == T_LEFT && grid[0][j] == T_UP)
                grid[i][j] = CROSS;
            else if(grid[i][0] == T_LEFT && grid[0][j] == HORIZONTAL)
                grid[i][j] = HORIZONTAL;
            else if(grid[i][0] == VERTICAL && grid[0][j] == T_UP)
                grid[i][j] = VERTICAL;
            else
                grid[i][j] = EMPTY;
        }
    }
}


void path_r(int to_r, int to_c, int from_r, int from_c)
{
    for(int i=0; i<GRID_H; i++)
    {
        for(int j=0; j<GRID_W; j++)
            path[i][j]=0;
    }

    int in_r = from_r;
    int in_c = from_c;

    while(!tile_path1(in_r,in_c,2))
    {

        path[in_r][in_c]=1;

        if(in_c<to_c)
            in_c++;
        else
            in_c--;
    }

    if(in_r<to_r)
    {
        for(; in_r<to_r; in_r++)
            path[in_r][in_c]=1;
    } else {
        for(; in_r>to_r; in_r--)
            path[in_r][in_c]=1;
    }

    while(!tile_path1(in_r,in_c,1))
    {
        /* printf("%d %d\n",in_r,in_c); */
        path[in_r][in_c]=1;

        if(in_r<to_r)
            in_r++;
        else
            in_r--;
    }

    if(in_c<to_c)
    {
        for(; in_c<=to_c; in_c++)
            path[in_r][in_c]=1;
    } else {
        for(; in_c>=to_c; in_c--)
            path[in_r][in_c]=1;
    }
}


int tile_path1(int from_r, int from_c,int how)
{
    int can;
    switch(grid[from_r][from_c])
    {
        case EMPTY:
            can = 0;
            break;

        case HORIZONTAL:
            can = 1;
            break;

        case VERTICAL:
            can = 2;
            break;

        case T_UP:
        case T_DOWN:
        case T_LEFT:
        case T_RIGHT:
        case CROSS:
        case TURN_UP_LEFT:
        case TURN_UP_RIGHT:
        case TURN_DOWN_LEFT:
        case TURN_DOWN_RIGHT:
            can = 3;
            break;

        default:
            can = 0;

    }

    if(how == 1 && (can == 1 || can == 3))
        return 1;
    else if(how == 2 && (can == 2 || can == 3))
        return 1;
    else
        return 0;
}
void ClearGridOutsideActive(int rows, int cols)
{
    for (int r = 0; r < GRID_H; r++) {
        for (int c = 0; c < GRID_W; c++) {
            if (r >= rows || c >= cols) {
                grid[r][c] = EMPTY;
            }
        }
    }
}
