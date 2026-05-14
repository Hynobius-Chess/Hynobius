#pragma once

#include "History_Heuristic.h"
#include "Killer_Move.h"
#include "PV_Table.h"
#include "Search_Variables.h"
#include "board/Board.h"
#include "evaluate/Evaluate.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include "move/Move_Order.h"
#include <chrono>

constexpr int MATE_SCORE = 30000;
constexpr int TIMEOUT_SCORE = 114514;
constexpr int MAX_SCORE = 1e9;

struct SearchStats
{
    int64_t depth;
    int score;
    int64_t negamaxNodes;
    int64_t qsNodes;
    int64_t timeMs;
    int64_t nps;

    int64_t ttProbe;
    int64_t ttHits;
    int64_t ttCuts;

    int64_t betaCuts;
    int64_t betaCutsFirst;

    int64_t failHighs;
    int64_t failHighsFirst;

    void clear()
    {
        *this = SearchStats{};
    }

    int64_t totalNodes()
    {
        return negamaxNodes + qsNodes;
    }
};

struct SearchResult
{
    bool isValid = 0;
    int bestScore;
    BitMove bestBitMove = INVALID_BITMOVE;

    SearchStats stats;

    PVTable pv;
};

struct SearchLimits
{
    int maxDepth = -1;
    int64_t maxTimeMs = -1;
};

struct SearchContext
{
    bool stopped = false;
    bool timeout = false;

    std::chrono::steady_clock::time_point startTime;

    PVTable pv, prevPv;

    killerMove kill;

    HistoryHeuristic history;

    int prevScore = 0;

    SearchStats stats;

    void reset()
    {
        stopped = false;
        timeout = false;
        startTime = std::chrono::steady_clock::now();
    }
};

class Search
{
public:
    Search(const Evaluate& _eval, const SearchLimits _limits);
    SearchResult findBestMove(const Board& board);

private:
    Evaluate eval;

    SearchResult chooseMove(Board& board,
                            const int depth,
                            const int alpha,
                            const int beta,
                            const int ply,
                            const BitMove PVMove);

    int negamax(Board& board, const int depth, const int alpha, const int beta, const int ply);

    int quietscence(Board& board, const int alpha, const int beta, const int ply);

    void printInfo();

    SearchLimits limits;

    SearchContext state;

    UndoState undoState[SearchVarialble::MAX_PLY + 5];

    BitMove moveBuffer[SearchVarialble::MAX_PLY + 5][256] = {INVALID_BITMOVE};

    bool shouldStop();

    int scoreMove(const Board& board, const BitMove move, const advanceMoves& adv);
};