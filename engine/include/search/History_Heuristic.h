#pragma once

#include "Type.h"
#include "move/Move.h"

struct HistoryHeuristic
{
    int table[2][64][64];

    inline void updateHisroty(const Player player, const BitMove move, const int depth)
    {
        Square from = getFromSquare(move);
        Square to = getToSquare(move);

        table[playerToIndex(player)][from][to] += depth * depth;
    }

    inline int getHistory(const Player player, const BitMove move)
    {
        Square from = getFromSquare(move);
        Square to = getToSquare(move);

        return table[playerToIndex(player)][from][to];
    }
};