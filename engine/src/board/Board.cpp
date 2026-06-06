#include "board/Board.h"
#include "board/Piece.h"
#include "search/Zobrist.h"

void computePiecePos(Board& board)
{
    for (int i = 1; i <= 12; i++)
    {
        board.pieceCount[i] = 0;
    }
    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            Piece p = board.board[r][c];
            if (p == Piece::EMPTY)
                continue;

            int pIndex = pieceToIndex(p);
            board.piecePos[pIndex][board.pieceCount[pIndex]++] = {r, c};
        }
    }
}

Board::Board()
{
    init();
}

void Board::init()
{
    player = Player::WHITE;

    for (int r = 0; r < 8; r++)
    {
        for (int c = 0; c < 8; c++)
        {
            board[r][c] = Piece::EMPTY;
        }
    }

    // Black pieces.
    board[0][0] = board[0][7] = Piece::BROOK;
    board[0][1] = board[0][6] = Piece::BKNIGHT;
    board[0][2] = board[0][5] = Piece::BBISHOP;
    board[0][3] = Piece::BQUEEN;
    board[0][4] = Piece::BKING;
    for (int c = 0; c < 8; c++)
    {
        board[1][c] = Piece::BPAWN;
    }

    // White pieces.
    board[7][0] = board[7][7] = Piece::WROOK;
    board[7][1] = board[7][6] = Piece::WKNIGHT;
    board[7][2] = board[7][5] = Piece::WBISHOP;
    board[7][3] = Piece::WQUEEN;
    board[7][4] = Piece::WKING;
    for (int c = 0; c < 8; c++)
    {
        board[6][c] = Piece::WPAWN;
    }

    computePiecePos(*this);

    materialScore = 0;
    PSTScore = 0;
    castleRights = 0b1111;

    zobristKey = computeZobrist(*this);

    enPassantPos = POS_NONE;

    clearRepetitionKey();
    pushRepetitionKey();
}