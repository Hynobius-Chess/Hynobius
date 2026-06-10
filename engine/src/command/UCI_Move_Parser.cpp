#include "board/Board.h"
#include "board/Piece.h"
#include "command/UCI_Move_Parcer.h"
#include "error/Engine_Error.h"
#include <string>

Move parseCastle(Move& move, const Board& board)
{
    Piece p = move.movePiece;

    if (p == Piece::WKING || p == Piece::BKING)
    {

        // white king side
        if (move.from.row == 0 && move.from.col == 4 && move.to.row == 0 && move.to.col == 6)
        {
            move.castle = Castle::SHORT_CASTLE;
        }

        // white queen side
        if (move.from.row == 0 && move.from.col == 4 && move.to.row == 0 && move.to.col == 2)
        {
            move.castle = Castle::LONG_CASTLE;
        }

        // black king side
        if (move.from.row == 7 && move.from.col == 4 && move.to.row == 7 && move.to.col == 6)
        {
            move.castle = Castle::SHORT_CASTLE;
        }

        // black queen side
        if (move.from.row == 7 && move.from.col == 4 && move.to.row == 7 && move.to.col == 2)
        {
            move.castle = Castle::LONG_CASTLE;
        }
    }

    return move;
}

Move parseEnPassant(Move& move, const Board& board)
{
    Piece p = move.movePiece;

    if (p != Piece::WPAWN && p != Piece::BPAWN)
        return move;

    if (board.enPassantPos == POS_NONE)
        return move;

    if (move.to != board.enPassantPos)
        return move;

    if (move.from.col == move.to.col)
        return move;

    if (board.at(move.to) != Piece::EMPTY)
        return move;

    move.isEnPassant = true;

    int capturedRow = (move.player == Player::WHITE ? move.to.row + 1 : move.to.row - 1);
    int capturedCol = move.to.col;

    move.capturePiece = board.board[capturedRow][capturedCol];

    return move;
}

Move parseUCIMove(const std::string strMove, const Board& board)
{
    if (strMove.size() != 4 && strMove.size() != 5)
    {
        throw UciMoveParserError(std::string("\'") + strMove + "\' is too short or too long. A valid move should be e2e4 or e7e8q" );
    }

    Move move;

    move.from = {8 - (strMove[1] - '0'), strMove[0] - 'a'};
    if (!isInBoard(move.from))
    {
        throw UciMoveParserError(std::string("\'") + strMove[0] + strMove[1] +
                                 "\' is out of board");
    }

    move.to = {8 - (strMove[3] - '0'), strMove[2] - 'a'};
    if (!isInBoard(move.to))
    {
        throw UciMoveParserError(std::string("\'") + strMove[2] + strMove[3] +
                                 "\' is out of board");
    }

    move.player = board.player;

    move.movePiece = board.at(move.from);
    if (!isValidPieceIndex(pieceToIndex(move.movePiece)))
    {
        throw UciMoveParserError(std::string("\'") + strMove.substr(0, 4) +
                                 "\' is not moving a piece");
    }

    move.capturePiece = board.board[move.to.row][move.to.col];

    move = parseCastle(move, board);
    move = parseEnPassant(move, board);

    if (strMove.length() == 5)
    {
        move.isPromotion = true;

        char promoChar = strMove[4];
        Player player = board.player;

        switch (promoChar)
        {
            case 'q':
                move.promotionPiece = makePiece(player, 'Q');
                break;
            case 'r':
                move.promotionPiece = makePiece(player, 'R');
                break;
            case 'b':
                move.promotionPiece = makePiece(player, 'B');
                break;
            case 'n':
                move.promotionPiece = makePiece(player, 'N');
                break;
            default:
                throw UciMoveParserError(std::string("\'") + strMove[4] +
                                         "\' is not a valid promotion piece");
        }
    }

    return move;
}