#pragma once

#include "Search_Variables.h"
#include "Type.h"
#include "debug/log.h"
#include "move/Move.h"

struct killerMove
{
    BitMove table[2][SearchVarialble::MAX_PLY] = {INVALID_BITMOVE};
    inline void addKillerMove(const BitMove move, const int ply)
    {
        ENGINE_ASSERT(ply < SearchVarialble::MAX_PLY);

        if (table[0][ply] == move)
            return;
        table[1][ply] = table[0][ply];
        table[0][ply] = move;
    }
};