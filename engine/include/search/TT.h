#pragma once

#include "Type.h"
#include <cstdint>

constexpr int TT_SIZE = 1 << 23;
constexpr uint64_t TT_MASK = TT_SIZE - 1;

enum TTFlag
{
    EXACT,
    LOWER,
    UPPER
};

struct TTEntry
{
    uint64_t key;
    int depth;
    int score;
    TTFlag flag;
    BitMove bestMove;
};

extern TTEntry TT[TT_SIZE];

inline int TTIndex(const uint64_t key)
{
    return key & TT_MASK;
}

bool probeTT(const uint64_t key,
             const int depth,
             const int alpha,
             const int beta,
             const int ply,
             TTEntry& TTOut,
             int& scoreOut,
             BitMove& moveOut,
             int64_t& hitCounter,
             int64_t& lowerCounter,
             int64_t& upperCounter,
             int64_t& exactCounter);

void storeTT(const uint64_t key,
             const int depth,
             const int ply,
             const int score,
             const TTFlag flag,
             const BitMove bestMove);