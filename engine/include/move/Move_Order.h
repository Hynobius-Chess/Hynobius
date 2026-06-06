#pragma once

#include "Type.h"
#include "board/Board.h"
#include <algorithm>

struct ScoreMove
{
    BitMove move;
    int score;
};

struct advanceMoves
{
    BitMove PVMove, TTMove, killerMove1, killerMove2;
};

template <typename scoreMoveFunction>
void sortMove(const Board& board, BitMove* moves, const int nMoves, scoreMoveFunction&& scoreMove)
{
    ScoreMove tmp[256];

    for (int i = 0; i < nMoves; i++)
    {
        tmp[i].move = moves[i];
        tmp[i].score = scoreMove(moves[i]);
    }

    std::sort(tmp,
              tmp + nMoves,
              [](const ScoreMove& a, const ScoreMove& b) { return a.score > b.score; });

    for (int i = 0; i < nMoves; i++)
    {
        moves[i] = tmp[i].move;
    }
}
