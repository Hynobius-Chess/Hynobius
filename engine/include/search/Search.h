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

struct SearchResult
{
    bool isValid = 0;
    int bestScore;
    BitMove bestBitMove = INVALID_BITMOVE;
};

struct SearchLimits
{
    int maxDepth = -1;
    int64_t maxTimeMs = -1;
};

struct SearchInfo
{
    int64_t depth, score, nodes, qsnodes, timeMs, nps;
    PVTable pv;
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

    SearchLimits limits;

    UndoState undoState[SearchVarialble::MAX_PLY + 5];

    BitMove moveBuffer[SearchVarialble::MAX_PLY + 5][256] = {INVALID_BITMOVE};

    struct SearchState
    {
        bool stopped = false;
        bool timeout = false;

        uint64_t negamaxNodes = 0;
        uint64_t qsNodes = 0;

        std::chrono::steady_clock::time_point startTime;

        PVTable pv, prevPv;

        killerMove kill;

        HistoryHeuristic history;

        int prevScore = 0;
    } state;

    bool shouldStop();

    int scoreMove(const Board& board, const BitMove move, const advanceMoves& adv);
};