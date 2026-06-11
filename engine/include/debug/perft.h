#pragma once

#include "board/Board.h"
#include <cstdint>

struct PerftStats
{
    uint64_t nodes = 0;
    uint64_t captures = 0;
    uint64_t enPassants = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
};

inline PerftStats& operator+=(PerftStats& a, const PerftStats& b)
{
    a.nodes += b.nodes;
    a.captures += b.captures;
    a.enPassants += b.enPassants;
    a.castles += b.castles;
    a.promotions += b.promotions;
    a.checks += b.checks;
    return a;
}

inline PerftStats operator+(PerftStats a, const PerftStats& b)
{
    a += b;
    return a;
}

// Return perft nodes
uint64_t perft(Board& board, int depth);

// Return `PerftStats` by running perft.
PerftStats perftWithStat(Board& board, int depth);