#pragma once

#include "Structure_IO.h"
#include "board/Board.h"
#include "board/Piece.h"
#include "debug/log.h"
#include "evaluate/Material_Point.h"
#include "search/Zobrist.h"

inline void checkBoardState(const Board& board)
{
#ifdef ENGINE_DEBUG
    // king must exist
    int whiteKing = 0;
    int blackKing = 0;

    // recompute material
    int recomputedMaterial = 0;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece p = board.board[r][c];

            if (p == Piece::EMPTY)
                continue;

            // king 檢查
            if (p == Piece::WKING)
                whiteKing++;
            if (p == Piece::BKING)
                blackKing++;

            // material
            recomputedMaterial += pieceValue(p) * (isWhite(p) ? 1 : -1);
        }
    }

    // check piece pos
    bool visi[8][8] = {};
    for (int i = 1; i <= 12; i++)
    {
        for (int j = 0; j < board.pieceCount[i]; j++)
        {
            auto [r, c] = board.piecePos[i][j];

            if (board.board[r][c] != static_cast<Piece>(i))
            {
                throw std::runtime_error("piece pos is incorrect");
            }

            if (visi[r][c] == 1)
            {
                throw std::runtime_error("piece pos has overlapped positions");
            }

            visi[r][c] = 1;
        }
    }

    // king invariant
    if (whiteKing != 1 || blackKing != 1)
    {
        DOUT("EVAL") << '\n' << board << '\n';
        ENGINE_FATAL("BOARD", "invalid king count ", "white=", whiteKing, " black=", blackKing);
    }

    // material invariant
    if (recomputedMaterial != board.materialScore)
    {
        DOUT("EVAL") << '\n' << board << '\n';
        ENGINE_FATAL("EVAL",
                     "material mismatch ",
                     "cached=",
                     board.materialScore,
                     " recomputed=",
                     recomputedMaterial);
    }

    // zobrist key
    ENGINE_ASSERT(computeZobrist(board) == board.zobristKey);
#endif
}