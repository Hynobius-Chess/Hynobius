#pragma once

#include "Type.h"
#include "debug/log.h"
#include "search/Search_Variables.h"

struct PVTable
{
    BitMove table[SearchVarialble::MAX_PLY][SearchVarialble::MAX_PLY];
    int length[SearchVarialble::MAX_PLY] = {}; // init to 0.

    inline void clearLine(int ply)
    {
        ENGINE_ASSERT(ply < SearchVarialble::MAX_PLY);
        length[ply] = 0;
    }

    inline void update(const int ply, const BitMove move)
    {
        table[ply][0] = move;

        for (int i = 0; i < length[ply + 1]; i++)
        {
            table[ply][i + 1] = table[ply + 1][i];
        }

        length[ply] = length[ply + 1] + 1;
    }
};