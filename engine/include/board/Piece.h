#pragma once

#include "debug/log.h"

#include <cctype>

// Define `Piece`.
enum class Piece
{
    EMPTY = 0,
    WPAWN,
    WKNIGHT,
    WBISHOP,
    WROOK,
    WQUEEN,
    WKING,
    BPAWN,
    BKNIGHT,
    BBISHOP,
    BROOK,
    BQUEEN,
    BKING,
    PIECE_COUNT
};

// Turn `Piece` into char.
inline constexpr char PIECE_TO_CHAR[]{
    '.', 'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

// Turn [`Player`][`charToPieceIndex`] into `Piece`.
inline constexpr Piece MAKE_PIECE_MAP[2][6]{
    {Piece::WPAWN, Piece::WKNIGHT, Piece::WBISHOP, Piece::WROOK, Piece::WQUEEN, Piece::WKING},
    {Piece::BPAWN, Piece::BKNIGHT, Piece::BBISHOP, Piece::BROOK, Piece::BQUEEN, Piece::BKING}};

// Turn char into index to support `MAKE_PIECE_MAP`.
// - Lower and upper letter are supported.
inline int charToPieceIndex(const char c)
{
    switch (std::toupper(c))
    {
        case 'P':
            return 0;
        case 'N':
            return 1;
        case 'B':
            return 2;
        case 'R':
            return 3;
        case 'Q':
            return 4;
        case 'K':
            return 5;
        default:
            ENGINE_FATAL("piece", "invalid input: ", c);
    }
}

// Turn `Piece` to `int` safely.
inline int pieceToIndex(const Piece p)
{
    return static_cast<int>(p);
}

// Check whether a piece is a piece.
inline bool isValidPieceIndex(const int index)
{
    return (1 <= index && index <= 12 ? true : false);
}

// Turn `Piece` to `char`.
inline char pieceToChar(const Piece p)
{
    ENGINE_ASSERT(p == Piece::EMPTY || isValidPieceIndex(pieceToIndex(p)));

    return PIECE_TO_CHAR[pieceToIndex(p)];
}

// Check whether a piece is white.
inline bool isWhite(const Piece p)
{
    if (p == Piece::EMPTY)
    {
        ENGINE_FATAL("piece", "empty piece does not have a color");
    }

    if (pieceToIndex(Piece::WPAWN) <= pieceToIndex(p) &&
        pieceToIndex(p) <= pieceToIndex(Piece::WKING))
        return true;
    else
        return false;
}

// Check whether a piece is black.
inline bool isBlack(const Piece p)
{
    if (p == Piece::EMPTY)
    {
        ENGINE_FATAL("piece", "empty piece does not have a color");
    }

    if (pieceToIndex(Piece::BPAWN) <= pieceToIndex(p) &&
        pieceToIndex(p) <= pieceToIndex(Piece::BKING))
        return true;
    else
        return false;
}

// Check whether two pieces share the same color.
inline bool isSameColor(const Piece p1, const Piece p2)
{
    if (p1 == Piece::EMPTY)
    {
        ENGINE_FATAL("piece", "Non-piece Piece::Object does not have a color");
    }
    if (p2 == Piece::EMPTY)
    {
        ENGINE_FATAL("piece", "Non-piece Piece::Object does not have a color");
    }

    return isWhite(p1) == isWhite(p2);
}