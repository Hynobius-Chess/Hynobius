#include "search/Search.h"
#include "board/Board.h"
#include "board/Check.h"
#include "debug/log.h"
#include "debug/validation.h"
#include "evaluate/Evaluate.h"
#include "evaluate/Material_Point.h"
#include "move/Generate_Move.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include "move/Move_Order.h"
#include "search/Search_Variables.h"
#include "search/TT.h"
#include "search/Zobrist.h"
#include <chrono>

const int TT_SCORE = 600000;
const int PVMOVE_SCORE = 500000;
const int PROMOTION_SCORE = 400000;
const int CAPTURE_SCORE = 300000;
const int KILLER_1_SCORE = 200000;
const int KILLER_2_SCORE = 100000;

int Search::scoreMove(const Board& board, const BitMove move, const advanceMoves& adv)
{
    int score = 0;

    if (adv.PVMove == move)
        return PVMOVE_SCORE;

    if (adv.TTMove == move)
        return TT_SCORE;

    if (getPromotion(move))
    {
        // 最先看
        score += PROMOTION_SCORE;
        score += pieceValue(getPromotePiece(move));
    }

    if (getCapture(move))
    {
        Piece capturePiece = getCapturePiece(board, move);

        ENGINE_ASSERT(capturePiece != Piece::EMPTY);

        Piece movePiece = getMovePiece(board, move);

        score += CAPTURE_SCORE;
        score += pieceValue(capturePiece) * 100 - pieceValue(movePiece);

        return score;
    }

    if (adv.killerMove1 == move)
        score += KILLER_1_SCORE;

    if (adv.killerMove2 == move)
        score += KILLER_2_SCORE;

    // history heuristic
    score += state.history.getHistory(board.player, move);

    return score;
}

void printInfo(const SearchInfo& info)
{
    std::cout << "info depth " << info.depth;

    if (info.score >= MATE_SCORE - SearchVarialble::MAX_SEARCH_DEPTH)
    {
        const int mate = (MATE_SCORE - info.score + 1) / 2;
        std::cout << " score mate " << mate;
    }
    else if (info.score <= -MATE_SCORE + SearchVarialble::MAX_SEARCH_DEPTH)
    {
        const int mate = (MATE_SCORE + info.score + 1) / 2;
        std::cout << " score mate -" << mate;
    }
    else
    {
        std::cout << " score cp " << info.score;
    }

    std::cout << " nodes " << info.nodes << " nps " << info.nps << " time " << info.timeMs;

    std::cout << " pv ";
    const int pvLength = info.pv.length[0];
    for (int i = 0; i < pvLength; i++)
    {
        std::cout << bitMoveToUCIMove(info.pv.table[0][i]) << ' ';
    }

    std::cout << '\n';
}

bool Search::shouldStop()
{
    if (state.stopped)
        return true;

    int interval = 2;
    if (limits.maxTimeMs <= 50)
        interval = 16;
    else if (limits.maxTimeMs <= 100)
        interval = 32;
    else if (limits.maxTimeMs <= 500)
        interval = 64;
    else if (limits.maxTimeMs <= 2500)
        interval = 128;
    else
        interval = 1024;

    if (((state.negamaxNodes + state.qsNodes) & (interval - 1)) != 0)
        return false;

    if (limits.maxTimeMs != -1)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - state.startTime).count();

        if (elapsed >= limits.maxTimeMs)
        {
            state.stopped = true;
            state.timeout = true;
            return true;
        }
    }

    return false;
}

// init function
Search::Search(const Evaluate& _eval, const SearchLimits _limits)
{
    eval = _eval;
    limits = _limits;
}

// public function to search for moves.
SearchResult Search::findBestMove(const Board& board)
{
    // init state.
    state = {false, false, 0, 0, std::chrono::steady_clock::now()};

    // make copyBoard non-const.
    Board copyBoard = board;

    checkBoardState(copyBoard);

    // Init repetition history
    copyBoard.pushRepetitionKey();

    // current result is invalid
    SearchResult result = {false, -MAX_SCORE, INVALID_BITMOVE};

    // At least output a valid move
    BitMove rootMoves[256];
    const int nRootMoves = generateAllLegalMoves(copyBoard, rootMoves);
    if (nRootMoves > 0)
    {
        result.isValid = true;
        result.bestBitMove = rootMoves[0];
        result.bestScore = -MAX_SCORE;
    }

    // No valid moves
    if (nRootMoves == 0)
    {
        result.isValid = true;
        result.bestBitMove = INVALID_BITMOVE;

        if (isInCheck(board, board.player))
            result.bestScore = -MATE_SCORE;
        else if (isInCheck(board, opponent(board.player)))
            result.bestScore = MATE_SCORE;
        else
            result.bestScore = 0;

        return result;
    }

    // set max depth.
    const int maxDepth =
        limits.maxDepth == -1 ? SearchVarialble::MAX_SEARCH_DEPTH : limits.maxDepth;

    BitMove lastBestMove = INVALID_BITMOVE;
    // iterative deepening
    for (int depth = 1; depth <= maxDepth; depth++)
    {
        // check time.
        if (shouldStop())
            break;

        SearchResult currentResult;
        if (depth == 1)
        {
            currentResult = chooseMove(copyBoard, depth, -MAX_SCORE, MAX_SCORE, 0, lastBestMove);
        }
        else
        {
            int window = 16;

            while (true)
            {
                // check time.
                if (shouldStop())
                    break;

                int alpha = state.prevScore - window;
                int beta = state.prevScore + window;

                currentResult = chooseMove(copyBoard, depth, alpha, beta, 0, lastBestMove);

                if (!currentResult.isValid)
                {
                    copyBoard.popRepetitionKey();
                    return result;
                }

                if (currentResult.bestScore <= alpha)
                {
                    window *= 2;
                    continue;
                }
                if (currentResult.bestScore >= beta)
                {
                    window *= 2;
                    continue;
                }

                break;
            }
        }

        if (currentResult.isValid)
        {
            result = currentResult;
            lastBestMove = result.bestBitMove;
            state.prevPv = state.pv;
            state.prevScore = result.bestScore;
        }

        // print info
        SearchInfo info;
        info.depth = depth;
        info.score = result.bestScore;
        info.nodes = state.negamaxNodes + state.qsNodes;
        info.qsnodes = state.qsNodes;

        auto now = std::chrono::steady_clock::now();
        info.timeMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - state.startTime).count();
        info.nps = (info.timeMs > 0 ? info.nodes * 1000 / info.timeMs : 0);
        info.pv = state.pv;

        printInfo(info);
    }

    copyBoard.popRepetitionKey();

    return result;
}

SearchResult
Search::chooseMove(Board& board, int depth, int alpha, int beta, int ply, const BitMove PVMove)
{
    SearchResult result = {false, -MAX_SCORE, INVALID_BITMOVE};

    // Clear currecnt PV line
    state.pv.clearLine(ply);

    // generate all moves
    BitMove* moves = moveBuffer[ply];
    int nMoves = generateAllLegalMoves(board, moves);

    // Get last PV move
    BitMove pvMove = INVALID_BITMOVE;
    if (state.prevPv.length[ply] > 0)
        pvMove = state.prevPv.table[ply][0];

    // sort moves
    advanceMoves adv = {
        pvMove, INVALID_BITMOVE, state.kill.table[0][ply], state.kill.table[1][ply]};
    sortMove(board, moves, nMoves, [&](BitMove move) { return scoreMove(board, move, adv); });

    for (int i = 0; i < nMoves; i++)
    {
        ENGINE_ASSERT(alpha < beta);

        // time check.
        if (shouldStop())
        {
            return {false, -MAX_SCORE, INVALID_BITMOVE};
        }

        const BitMove move = moves[i];

        doBitMove(board, move, undoState[ply]);
        board.pushRepetitionKey();

        const int score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);

        board.popRepetitionKey();
        undoBitMove(board, move, undoState[ply]);

        const Move oriMove = bitMovetoOriMove(board, move);
        // std::cout << oriMove << " | " << score << '\n';

        if (score == -TIMEOUT_SCORE)
        {
            return {false, -MAX_SCORE, INVALID_BITMOVE};
        }
        if (score > result.bestScore)
        {
            result.isValid = true;
            result.bestScore = score;
            result.bestBitMove = move;
        }
        if (score > alpha)
        {
            alpha = score;
            state.pv.update(ply, move);
        }
        if (alpha >= beta)
            break;
    }

    return result;
}

int Search::negamax(Board& board, int depth, int alpha, int beta, int ply)
{
    state.negamaxNodes++;

    // Clear current PV line
    state.pv.clearLine(ply);

    // Check repetition
    if (board.isRepetition())
        return 0;

    // Probe TT table.
    TTEntry ttOut;
    int ttScore = -MAX_SCORE;
    BitMove ttMove = INVALID_BITMOVE;
    if (probeTT(board.zobristKey, depth, alpha, beta, ply, ttOut, ttScore, ttMove) == true)
    {
        return ttScore;
    }

    // Record original alpha for TT store.
    const int oriAlpha = alpha;

    int bestScore = -MAX_SCORE;
    BitMove bestMove = INVALID_BITMOVE;

    if (shouldStop())
        return TIMEOUT_SCORE;

    // depth = 0 -> qs search
    if (depth == 0)
    {
        return quietscence(board, alpha, beta, ply);
    }

    // generate all moves
    BitMove* moves = moveBuffer[ply];
    const int nMoves = generateAllLegalMoves(board, moves);

    // Get last PV move.
    BitMove pvMove = INVALID_BITMOVE;
    if (state.prevPv.length[ply] > 0)
        pvMove = state.prevPv.table[ply][0];

    // Sort moves.
    advanceMoves adv = {pvMove, ttMove, state.kill.table[0][ply], state.kill.table[1][ply]};
    sortMove(board, moves, nMoves, [&](BitMove move) { return scoreMove(board, move, adv); });

    // check checkmate / stalemate
    if (nMoves == 0)
    {
        const int score = isInCheck(board, board.player) ? -MATE_SCORE + ply : 0;
        return score;
    }

    for (int i = 0; i < nMoves; i++)
    {
        ENGINE_ASSERT(alpha < beta);

        // time check.
        if (shouldStop())
            return TIMEOUT_SCORE;

        const BitMove move = moves[i];

        doBitMove(board, move, undoState[ply]);
        board.pushRepetitionKey();

        bool doLMR = false;

        if (i >= 4 &&                    // after the fifth move -> reduce search depth.
            depth >= 3 &&                // LMR is only for deep nodes.
            !undoState[ply].isCapture && // don't reduce capture moves.
            !undoState[ply].isPromotion  // don't reduce promotions.
        )
        {
            // after doBitMove, the player stored in board is already the enemy.
            if (!isInCheck(board, board.player))
            {
                doLMR = true;
            }
        }

        int score = -MAX_SCORE;

        if (doLMR)
        {
            const int searchDepth = depth - 2;

            // Using null-window to limit score window -> faster.
            score = -negamax(board, searchDepth, -alpha - 1, -alpha, ply + 1);
            if (score != -TIMEOUT_SCORE && score > alpha)
            {
                // fail high -> research with full depth.
                score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
            }
        }
        else
        {
            score = -negamax(board, depth - 1, -beta, -alpha, ply + 1);
        }

        board.popRepetitionKey();
        undoBitMove(board, move, undoState[ply]);

        // time check
        if (score == -TIMEOUT_SCORE)
            return TIMEOUT_SCORE;

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
        if (score > alpha)
        {
            alpha = score;

            // Update PV line
            state.pv.update(ply, move);
        }

        if (alpha >= beta)
        {
            if (!undoState[ply].isCapture)
            {
                state.kill.addKillerMove(move, ply);
                state.history.updateHisroty(undoState[ply].player, move, depth);
            }
            break;
        }
    }

    // define TT flag to store.
    TTFlag flag;
    if (bestScore <= oriAlpha)
        flag = UPPER;
    else if (bestScore >= beta)
        flag = LOWER;
    else
        flag = EXACT;

    // store to TT table.
    storeTT(board.zobristKey, depth, ply, bestScore, flag, bestMove);

    return bestScore;
}

// quietscence search (determine time complexity)
int Search::quietscence(Board& board, int alpha, int beta, int ply)
{
    state.qsNodes++;

    if (shouldStop())
        return TIMEOUT_SCORE;

    if (ply >= SearchVarialble::MAX_PLY)
    {
        return (board.player == Player::WHITE ? 1 : -1) *
               eval.evaluateBoard(board, EVALUATE_MODE::FULL);
    }

    BitMove* moves = moveBuffer[ply];
    int nMoves = -1;

    // check evasion
    if (isInCheck(board, board.player))
    {
        nMoves = generateAllLegalMoves(board, moves);

        if (nMoves == 0)
        {
            return -MATE_SCORE + ply;
        }
    }
    else
    {
        int standerdPoint = (board.player == Player::WHITE ? 1 : -1) *
                            eval.evaluateBoard(board, EVALUATE_MODE::FULL);
        if (standerdPoint >= beta)
            return standerdPoint;
        if (standerdPoint > alpha)
            alpha = standerdPoint;

        nMoves = generateLegalCaptureMoves(board, moves);

        if (nMoves == 0)
        {
            return alpha;
        }
    }

    // Sort moves.
    advanceMoves adv = {
        INVALID_BITMOVE,
        INVALID_BITMOVE,
        INVALID_BITMOVE,
        INVALID_BITMOVE,
    };
    sortMove(board, moves, nMoves, [&](BitMove move) { return scoreMove(board, move, adv); });

    for (int i = 0; i < nMoves; i++)
    {
        ENGINE_ASSERT(alpha < beta);
        
        // time check.
        if (shouldStop())
            return TIMEOUT_SCORE;

        const BitMove move = moves[i];

        // recursive
        doBitMove(board, move, undoState[ply]);

        const int score = -quietscence(board, -beta, -alpha, ply + 1);

        undoBitMove(board, move, undoState[ply]);

        // time check.
        if (score == -TIMEOUT_SCORE)
            return TIMEOUT_SCORE;

        if (score >= beta)
            return score;
        if (score > alpha)
            alpha = score;
    }

    return alpha;
}