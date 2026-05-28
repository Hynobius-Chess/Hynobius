#include "debug/perft.h"
#include "board/Check.h"
#include "debug/validation.h"
#include "move/Generate_Move.h"
#include "move/Make_BitMove.h"

PerftStats perftWithStat(Board& board, int depth)
{
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    PerftStats stat;

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);

        if (depth == 1)
        {
            stat.nodes++;
            stat.captures += (getCapture(move) == true ? 1u : 0u);
            stat.enPassants += (getEnPassant(move) == true ? 1u : 0u);
            stat.castles += (getCastle(move) == true ? 1u : 0u);
            stat.promotions += (getPromotion(move) == true ? 1u : 0u);
            // player now is the enemy because we've functioned doBitMove.
            stat.checks += (isInCheck(board, board.player) ? 1u : 0u);
        }
        else
        {
            stat += perftWithStat(board, depth - 1);
        }

        undoBitMove(board, move, undo);

        // checkBoardState(board);
    }

    return stat;
}

uint64_t perft(Board& board, int depth)
{
    BitMove moves[256];
    int nMoves = generateAllLegalMoves(board, moves);

    uint64_t node = 0;

    if (depth == 0)
    {
        return 1;
    }
    if (depth == 1)
    {
        return static_cast<uint64_t>(nMoves);
    }

    for (int i = 0; i < nMoves; i++)
    {
        BitMove move = moves[i];

        UndoState undo;

        doBitMove(board, move, undo);

        node += perft(board, depth - 1);

        undoBitMove(board, move, undo);

        checkBoardState(board);
    }

    return node;
}
