#include "evaluate/Mobility.h"
#include "move/Generate_Position.h"

int evaluateKnightMobility(const Board& board, Player player, int knightMobilityWeight)
{
    int cnt = 0;
    Piece knight = makePiece(player, 'N');
    int knightCount = board.getPieceCount(knight);
    const auto posArray = board.getPiecePos(knight);

    for (int i = 0; i < knightCount; i++)
    {
        Position knightPositions[32];
        generatePiecePosFromPos(board, posArray[i], knight, [&](const Position pos) { cnt++; });
    }

    return cnt * knightMobilityWeight;
}

int evaluateBishopMobility(const Board& board, Player player, int bishopMobilityWeight)
{
    int cnt = 0;
    Piece bishop = makePiece(player, 'N');
    int bishopCount = board.getPieceCount(bishop);
    const auto posArray = board.getPiecePos(bishop);

    for (int i = 0; i < bishopCount; i++)
    {
        Position bishopPositions[32];
        generatePiecePosFromPos(board, posArray[i], bishop, [&](const Position pos) { cnt++; });
    }

    return cnt * bishopMobilityWeight;
}

int evaluateRookMobility(const Board& board, Player player, int rookMobilityWeight)
{
    int cnt = 0;
    Piece rook = makePiece(player, 'N');
    int rookCount = board.getPieceCount(rook);
    const auto posArray = board.getPiecePos(rook);

    for (int i = 0; i < rookCount; i++)
    {
        Position rookPositions[32];
        generatePiecePosFromPos(board, posArray[i], rook, [&](const Position pos) { cnt++; });
    }

    return cnt * rookMobilityWeight;
}