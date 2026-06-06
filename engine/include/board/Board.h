#pragma once

#include "Piece.h"
#include "Type.h"
#include "board/Piece.h"
#include "debug/log.h"
#include "search/Search_Variables.h"
#include <cstdint>

// Store direct position like `board[pos.row][pos.col]`.
struct Position
{
    int row = -1;
    int col = -1;

    bool operator==(const Position& other) const
    {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position& other) const
    {
        return !(*this == other);
    }
};

// invalid position.
constexpr Position POS_NONE = {-1, -1};

// WARN temporary transfrom fuction
inline Position squareToPosition(const Square square)
{
    return {square / 8, square & 7};
}

// WARN temporary transfrom fuction
inline Square positionToSquare(const Position pos)
{
    return static_cast<Square>(pos.row * 8 + pos.col);
}

inline bool isValidSquare(const Square square)
{
    return ((0 <= square && square < 64) ? true : false);
}

// Check whether a position is inside board. (8 x 8)
inline bool isInBoard(const Position pos)
{
    return 0 <= pos.row && pos.row < 8 && 0 <= pos.col && pos.col < 8;
}
inline bool isInBoard(const int row, const int col)
{
    return 0 <= row && row < 8 && 0 <= col && col < 8;
}

// Check whether a player is `Player::WHITE` or `Player::BLACK`.
// WARN useless
inline bool isPlayerValid(const Player player)
{
    return player == Player::WHITE || player == Player::BLACK;
}

// Returns the opposide player.
inline Player opponent(const Player player)
{
    return (player == Player::WHITE ? Player::BLACK : Player::WHITE);
}

/*
Make `Piece` using `Player` and `char`.
Note that LOWERCASE chars are not accepted.
*/
inline Piece makePiece(const Player player, const char pieceChar)
{
    return MAKE_PIECE_MAP[static_cast<int>(player)][charToPieceIndex(pieceChar)];
}

/*
Returns `static_cast<int>(player)`
*/
inline int playerToIndex(const Player player)
{
    return static_cast<int>(player);
}

/*
The main body of board.

Responsibilities:
- store must-have info like `Piece` and `Player`.
- store info to speed up modules.

Invariants:
- `piecePos` must be correct.
- info to speed up modules must be correct.
*/
struct Board
{
    Board();

    /*
    Initialize `board` with `startpos`.

    Invariants:
    - every info stored in `board` must be initialized.
    - using other FEN or PGN to initialize board is NOT contained here.

    WARN TODO init() should be compatible with FEN and PGN
    */
    void init();

    /*
    Returns the Piece at `pos`.

    Obsoleting.
    */
    inline Piece at(Position pos) const
    {
        ENGINE_ASSERT(isInBoard(pos));
        return board[pos.row][pos.col];
    }

    // Set the `pos` in the board to piece `p`.
    inline void set(Position pos, Piece p)
    {
        board[pos.row][pos.col] = p;
    }

    // Store pieces on the board.
    Piece board[8][8];

    int materialScore;
    int PSTScore;
    Player player;

    /*
    Store castling rights using bits.
    - 0001 black king  side
    - 0010 black queen side
    - 0100 white king  side
    - 1000 white queen side
    */
    int castleRights;

    uint64_t zobristKey;

    Position enPassantPos;

    uint64_t keyHistory[SearchVarialble::MAX_GAME_PLY];
    int repetitionHistoryLength = 0;

    inline void clearRepetitionKey()
    {
        repetitionHistoryLength = 0;
    }

    inline void pushRepetitionKey()
    {
        ENGINE_ASSERT(repetitionHistoryLength < SearchVarialble::MAX_GAME_PLY);
        keyHistory[repetitionHistoryLength++] = zobristKey;
    }

    inline void popRepetitionKey()
    {
        ENGINE_ASSERT(repetitionHistoryLength != 0);
        repetitionHistoryLength--;
    }

    // Store piece positions by their piece type.
    Position piecePos[13][64];

    // Store the number of every pieces.
    int pieceCount[13] = {};

    // Delete the piece in `target` in `piecePos[Piece]`.
    inline void piecePosDelete(Position* posArray, int& count, const Position target)
    {
        for (int i = 0; i < count; i++)
        {
            if (posArray[i] == target)
            {
                posArray[i] = posArray[count - 1];
                count--;
                return;
            }
        }

        ENGINE_FATAL("board", "can't find piece position");
    }

    // Add the Piece in `add` in `piecePos[Piece]`.
    inline void piecePosAdd(Position* posArray, int& count, const Position add)
    {
        posArray[count++] = add;
    }

    // Returns the array storing piece poitions by piece `p`. WARN the name does not mean it returns
    // an array.
    inline const Position* getPiecePos(const Piece p) const
    {
        return piecePos[pieceToIndex(p)];
    }

    // Returns the number of piece `p`.
    inline int getPieceCount(const Piece p) const
    {
        if (!isValidPieceIndex(pieceToIndex(p)))
        {
            ENGINE_FATAL(
                "board",
                "Non-piece Piece::Object can't get piece count because it is not on the board.");
        }

        return pieceCount[pieceToIndex(p)];
    }

    inline bool isRepetition()
    {
        int count = 0;
        for (int i = repetitionHistoryLength - 1; i > 0; i--)
        {
            if (zobristKey == keyHistory[i])
            {
                count++;

                if (count >= 2)
                {
                    return true;
                }
            }
        }

        return false;
    }
};

// Check whether every piece positions is correct.
// WARN useless
bool validatePiecePos(const Board& b);

// Generate every piece positions by the current board.
void computePiecePos(Board& board);