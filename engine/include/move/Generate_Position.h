#pragma once

#include "board/Board.h"
#include "move/Move_Direction.h"

template <typename Emit>
void generatePosFromPosWithJumpPiece(const Board& board,
                                     const Position& pos,
                                     Piece movePiece,
                                     const int dr[],
                                     const int dc[],
                                     Emit&& emit)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    for (int i = 0; i < 8; i++)
    {
        const int row = pos.row + dr[i], col = pos.col + dc[i];
        if (!isInBoard(row, col))
            continue;

        Piece atPiece = board.board[row][col];
        if (atPiece != Piece::EMPTY && isSameColor(atPiece, movePiece))
            continue;

        emit(Position{row, col});
    }
}

template <typename Emit>
void generatePosFromPosWithSlidePiece(const Board& board,
                                      const Position& pos,
                                      Piece movePiece,
                                      const int dr[],
                                      const int dc[],
                                      Emit&& emit)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    for (int i = 0; i < 4; i++)
    {
        int row = pos.row + dr[i], col = pos.col + dc[i];

        while (true)
        {
            if (!isInBoard(row, col))
                break;
            Piece atPiece = board.board[row][col];
            if (atPiece != Piece::EMPTY && isSameColor(atPiece, movePiece))
                break;

            emit(Position{row, col});

            if (atPiece != Piece::EMPTY)
                break;

            row += dr[i];
            col += dc[i];
        }
    }
}

template <typename Emit>
void generateCaptureFromPosWithJumpPiece(const Board& board,
                                         const Position& pos,
                                         Piece movePiece,
                                         const int dr[],
                                         const int dc[],
                                         Emit&& emit)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    for (int i = 0; i < 8; i++)
    {
        const int row = pos.row + dr[i], col = pos.col + dc[i];
        if (!isInBoard(row, col))
            continue;

        Piece atPiece = board.board[row][col];
        if (atPiece == Piece::EMPTY)
            continue;
        if (isSameColor(atPiece, movePiece))
            continue;

        emit(Position{row, col});
    }
}

template <typename Emit>
int generateCaptureFromPosWithSlidePiece(const Board& board,
                                         const Position& pos,
                                         Piece movePiece,
                                         const int dr[],
                                         const int dc[],
                                         Emit&& emit)
{
    ENGINE_ASSERT(isInBoard(pos));
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    int cnt = 0;

    for (int i = 0; i < 4; i++)
    {
        int row = pos.row + dr[i], col = pos.col + dc[i];

        while (true)
        {
            if (!isInBoard(row, col))
                break;
            Piece atPiece = board.board[row][col];

            if (atPiece != Piece::EMPTY && isSameColor(atPiece, movePiece))
                break;

            if (atPiece != Piece::EMPTY)
            {
                emit(Position{row, col});
                break;
            }

            row += dr[i];
            col += dc[i];
        }
    }

    return cnt;
}

template <typename Emit>
void generatePiecePosFromPos(const Board& board, const Position pos, const Piece p, Emit&& emit)
{
    ENGINE_ASSERT(isInBoard(pos));

    switch (p)
    {
        case Piece::WKNIGHT:
        case Piece::BKNIGHT:
            generatePosFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KNIGHT_DR, MoveDirection::KNIGHT_DC, emit);
            return;

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            generatePosFromPosWithSlidePiece(board,
                                             pos,
                                             p,
                                             MoveDirection::BISHOP_QUEEN_DR,
                                             MoveDirection::BISHOP_QUEEN_DC,
                                             emit);
            return;

        case Piece::WROOK:
        case Piece::BROOK:
            generatePosFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, emit);
            return;

        case Piece::WQUEEN:
        case Piece::BQUEEN:
        {
            generatePosFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, emit);
            generatePosFromPosWithSlidePiece(board,
                                             pos,
                                             p,
                                             MoveDirection::BISHOP_QUEEN_DR,
                                             MoveDirection::BISHOP_QUEEN_DC,
                                             emit);
        }
            return;

        case Piece::WKING:
        case Piece::BKING:
            generatePosFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KING_DR, MoveDirection::KING_DC, emit);
            return;

        case Piece::WPAWN:
        case Piece::BPAWN:
            ENGINE_FATAL("generate position", "pawns are not pieces.");

        case Piece::EMPTY:
            ENGINE_FATAL("generate position", "empty is not a piece.");
        case Piece::PIECE_COUNT:
            ENGINE_FATAL("generate position", "piece_count is not a piece.");
    }
}

template <typename Emit>
void generatePieceCaptureFromPos(const Board& board, const Position pos, const Piece p, Emit&& emit)
{
    ENGINE_ASSERT(isInBoard(pos));

    switch (p)
    {
        case Piece::WKNIGHT:
        case Piece::BKNIGHT:
            generateCaptureFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KNIGHT_DR, MoveDirection::KNIGHT_DC, emit);
            return;

        case Piece::WBISHOP:
        case Piece::BBISHOP:
            generateCaptureFromPosWithSlidePiece(board,
                                                 pos,
                                                 p,
                                                 MoveDirection::BISHOP_QUEEN_DR,
                                                 MoveDirection::BISHOP_QUEEN_DC,
                                                 emit);
            return;

        case Piece::WROOK:
        case Piece::BROOK:
            generateCaptureFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, emit);
            return;

        case Piece::WQUEEN:
        case Piece::BQUEEN:
            generateCaptureFromPosWithSlidePiece(
                board, pos, p, MoveDirection::ROOK_QUEEN_DR, MoveDirection::ROOK_QUEEN_DC, emit);
            generateCaptureFromPosWithSlidePiece(board,
                                                 pos,
                                                 p,
                                                 MoveDirection::BISHOP_QUEEN_DR,
                                                 MoveDirection::BISHOP_QUEEN_DC,
                                                 emit);
            return;

        case Piece::WKING:
        case Piece::BKING:
            generateCaptureFromPosWithJumpPiece(
                board, pos, p, MoveDirection::KING_DR, MoveDirection::KING_DC, emit);
            return;

        case Piece::WPAWN:
        case Piece::BPAWN:
            ENGINE_FATAL("generate position", "pawns are not pieces.");

        case Piece::EMPTY:
            ENGINE_FATAL("generate position", "empty is not a piece.");
        case Piece::PIECE_COUNT:
            ENGINE_FATAL("generate position", "piece_count is not a piece.");
    }
}