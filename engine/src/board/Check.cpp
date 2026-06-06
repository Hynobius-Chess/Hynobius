#include "board/Check.h"
#include "board/Attack.h"
#include "board/Board.h"

// Check whether the king of the player is attacked.
bool isInCheck(const Board& board, const Player player)
{
    const Piece king = makePiece(player, 'K');
    const int kingCount = board.getPieceCount(king);
    const auto* posArray = board.getPiecePos(king);

    ENGINE_ASSERT(kingCount == 1);

    if (isSquareAttacked(board, posArray[0], opponent(player)))
        return true;
    else
        return false;
}