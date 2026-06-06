#pragma once
#include "board/Board.h"
#include "move/Move.h"
#include <board/Piece.h>
#include <cstdint>

// Get the piece the move captured.
inline Piece getCapturePiece(const Board& board, const BitMove move)
{
    if (!getCapture(move))
        return Piece::EMPTY;

    Position from = squareToPosition(getFromSquare(move));
    Position to = squareToPosition(getToSquare(move));
    Piece res;
    if (getEnPassant(move))
    {
        Position capturePos = {from.row, to.col};
        res = board.at(capturePos);

        ENGINE_ASSERT(res == makePiece(opponent(board.player), 'P'));
    }
    else
    {
        res = board.at(to);
    }

    return res;
}

inline Piece getMovePiece(const Board& board, const BitMove move)
{
    Position from = squareToPosition(getFromSquare(move));
    Piece movePiece = board.at(from);

    ENGINE_ASSERT(isValidPieceIndex(pieceToIndex(movePiece)));

    return movePiece;
}

// intergret move infos
// placedPiece: The piece that actually placed at Position to
struct MoveState
{
    Position from, to;

    Piece movePiece, capturedPiece, placedPiece;

    bool isCastle, isPromotion, isCapture, isEnPassant;

    Player player;

    int castleRights;

    MoveState(const Board& board, const BitMove move)
    {
        from = squareToPosition(getFromSquare(move));
        to = squareToPosition(getToSquare(move));

        movePiece = board.at(from);
        capturedPiece = getCapturePiece(board, move);

        isCastle = getCastle(move);
        isPromotion = getPromotion(move);
        isCapture = getCapture(move);

        if (isPromotion)
        {
            placedPiece = getPromotePiece(move);
        }
        else
        {
            placedPiece = movePiece;
        }

        player = board.player;

        castleRights = board.castleRights;

        isEnPassant = getEnPassant(move);
    }
};

struct UndoState
{
    Piece capturedPiece;
    Piece movePiece;
    Piece placedPiece;
    Position from;
    Position to;
    bool isCastle;
    bool isPromotion;
    bool isEnPassant;
    bool isCapture;
    Position enPassantPos;

    int castleRights;
    int materialScore;
    int PSTScore;
    uint64_t zobristKey;

    Player player;

    inline void make(const Board& board, const MoveState& state)
    {
        capturedPiece = state.capturedPiece;
        movePiece = state.movePiece;
        placedPiece = state.placedPiece;
        from = state.from;
        to = state.to;
        isCastle = state.isCastle;
        isPromotion = state.isPromotion;
        isEnPassant = state.isEnPassant;
        isCapture = state.isCapture;
        enPassantPos = board.enPassantPos;
        castleRights = board.castleRights;
        materialScore = board.materialScore;
        PSTScore = board.PSTScore;
        zobristKey = board.zobristKey;
        player = board.player;
    }
};

void doBitMove(Board& board, const BitMove move, UndoState& undo);

void undoBitMove(Board& board, const BitMove move, const UndoState& undo);