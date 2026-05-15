#include "search/TT.h"
#include "Type.h"
#include "search/Search.h"

#include <assert.h>
#include <cstdint>

TTEntry TT[TT_SIZE];

bool probeTT(uint64_t key,
             int depth,
             int alpha,
             int beta,
             int ply,
             TTEntry& TTOut,
             int& scoreOut,
             BitMove& moveOut,
             int64_t& hitCounter,
             int64_t& lowerCounter,
             int64_t& upperCounter,
             int64_t& exactCounter
            )
{
    TTEntry& tt = TT[TTIndex(key)];

    // stop when collide
    if (tt.key != key)
        return false;
    // stop with lower depth
    if (tt.depth < depth)
        return false;

    hitCounter++;

    int score = tt.score;
    TTOut = tt;
    moveOut = tt.bestMove;

    if (score >= MATE_SCORE - 1000)
    {
        score -= ply;
    }
    else if (score <= -MATE_SCORE + 1000)
    {
        score += ply;
    }

    if (tt.flag == EXACT)
    {
        exactCounter++;

        scoreOut = score;
        return true;
    }
    if (tt.flag == LOWER && score >= beta)
    {
        lowerCounter++;
        scoreOut = score;
        return true;
    }
    if (tt.flag == UPPER && score <= alpha)
    {
        upperCounter++;
        scoreOut = score;
        return true;
    }

    return false;
}

void storeTT(uint64_t key, int depth, int ply, int score, TTFlag flag, BitMove bestMove)
{
    TTEntry& tt = TT[TTIndex(key)];

    // depth check
    if (tt.key == key && depth < tt.depth)
        return;

    if (score >= MATE_SCORE - 1000)
    {
        score += ply;
    }
    else if (score <= -MATE_SCORE + 1000)
    {
        score -= ply;
    }

    tt.key = key;
    tt.depth = depth;
    tt.score = score;
    tt.flag = flag;
    tt.bestMove = bestMove;
}