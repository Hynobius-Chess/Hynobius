#include "fen/FEN_Parser.h"
#include "board/Board.h"
#include "error/Engine_Error.h"
#include "evaluate/Material_Point.h"
#include "evaluate/PST.h"
#include "search/Zobrist.h"
#include <iostream>
#include <sstream>

Board fenToBoard(const std::string& fen)
{
    Board board;

    std::istringstream iss(fen);
    std::string boardStr, player, castling, enpass, halfmove, fullmove;
    if (!(iss >> boardStr >> player >> castling >> enpass >> halfmove >> fullmove))
    {
        throw FenParseError("FEN in incomplete. It should contain: board, player, castling rights, "
                            "en passant square, half move counter, full move counter");
    }

    int row = 0, col = 0;
    for (auto c : boardStr)
    {
        if (c == '/')
        {
            if (col != 8)
                throw FenParseError("bad board layout");
            row++;
            col = 0;
        }

        else if (isdigit(c))
        {
            for (int i = 0; i < c - '0'; i++)
            {
                if (!isInBoard({row, col + i}))
                    throw FenParseError("bad board layout");
                board.board[row][col + i] = Piece::EMPTY;
            }
            col += c - '0';
        }

        else
        {
            if (!isInBoard({row, col}))
                throw FenParseError("bad board layout");
            board.board[row][col++] = makePiece((isupper(c) ? Player::WHITE : Player::BLACK), c);
        }
    }
    if (!(row == 7 && col == 8))
        throw FenParseError("bad board layout");

    if (!(player == "w" || player == "b"))
        throw FenParseError("bad player");

    board.player = (player == "w" ? Player::WHITE : Player::BLACK);

    board.castleRights = 0;
    for (char c : castling)
    {
        switch (c)
        {
            case 'K':
                board.castleRights |= 0b0100;
                break;
            case 'Q':
                board.castleRights |= 0b1000;
                break;
            case 'k':
                board.castleRights |= 0b0001;
                break;
            case 'q':
                board.castleRights |= 0b0010;
                break;
            case '-':
                break;
            default:
                throw FenParseError("bad castling rights");
                break;
        }
    }

    if (enpass != "-")
    {
        board.enPassantPos = {8 - (enpass[1] - '0'), (enpass[0] - 'a')};

        if (!isInBoard(board.enPassantPos))
            throw FenParseError("bad en passant square");
    }
    else
    {
        board.enPassantPos = POS_NONE;
    }

    // initialize variables.
    board.materialScore = computePieceValue(board);
    board.PSTScore = computePST(board);
    board.zobristKey = computeZobrist(board);
    computePiecePos(board);

    board.clearRepetitionKey();
    board.pushRepetitionKey();

    return board;
}