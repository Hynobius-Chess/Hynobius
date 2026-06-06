#pragma GCC optimize("O3,unroll-loops")

#include "board/Attack.h"
#include "board/Board.h"
#include "move/Move_Direction.h"

int countPawnAttacks(const Board& board, Position pos, const Player player)
{
    const int pawnRow = pos.row + (player == Player::WHITE ? 1 : -1);
    const Piece pawn = makePiece(player, 'P');

    int cnt = 0;
    const int pawnLeft = pos.col - 1;
    if (isInBoard(pawnRow, pawnLeft) && board.board[pawnRow][pawnLeft] == pawn)
        cnt++;

    const int pawnRight = pos.col + 1;
    if (isInBoard(pawnRow, pawnRight) && board.board[pawnRow][pawnRight] == pawn)
        cnt++;

    return cnt;
}

bool isPawnAttacking(const Board& board, Position pos, const Player player)
{
    const int pawnRow = pos.row + (player == Player::WHITE ? 1 : -1);
    const Piece pawn = makePiece(player, 'P');

    const int pawnLeft = pos.col - 1;
    if (isInBoard(pawnRow, pawnLeft) && board.board[pawnRow][pawnLeft] == pawn)
        return true;

    const int pawnRight = pos.col + 1;
    if (isInBoard(pawnRow, pawnRight) && board.board[pawnRow][pawnRight] == pawn)
        return true;

    return false;
}

int countKingAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    const Piece king = makePiece(player, 'K');

    for (int i = 0; i < 8; i++)
    {
        const int row = pos.row + MoveDirection::KING_DR[i];
        const int col = pos.col + MoveDirection::KING_DC[i];

        if (isInBoard(row, col) && board.board[row][col] == king)
            cnt++;
    }

    return cnt;
}

bool isKingAttacking(const Board& board, Position pos, const Player player)
{
    const Piece king = makePiece(player, 'K');

    for (int i = 0; i < 8; i++)
    {
        const int row = pos.row + MoveDirection::KING_DR[i];
        const int col = pos.col + MoveDirection::KING_DC[i];

        if (isInBoard(row, col) && board.board[row][col] == king)
            return true;
    }

    return false;
}

int countKnightAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    const Piece knight = makePiece(player, 'N');

    for (int i = 0; i < 8; i++)
    {
        const int row = pos.row + MoveDirection::KNIGHT_DR[i];
        const int col = pos.col + MoveDirection::KNIGHT_DC[i];

        if (isInBoard(row, col) && board.board[row][col] == knight)
            cnt++;
    }

    return cnt;
}

bool isKnightAttacking(const Board& board, Position pos, const Player player)
{
    const Piece knight = makePiece(player, 'N');

    for (int i = 0; i < 8; i++)
    {
        const int row = pos.row + MoveDirection::KNIGHT_DR[i];
        const int col = pos.col + MoveDirection::KNIGHT_DC[i];

        if (isInBoard(row, col) && board.board[row][col] == knight)
            return true;
    }

    return false;
}

int countDiagonalAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    const Piece bishop = makePiece(player, 'B');
    const Piece queen = makePiece(player, 'Q');

    for (int i = 0; i < 4; i++)
    {
        int row = pos.row + MoveDirection::BISHOP_QUEEN_DR[i];
        int col = pos.col + MoveDirection::BISHOP_QUEEN_DC[i];

        while (true)
        {
            if (!isInBoard(row, col))
                break;

            const Piece pp = board.board[row][col];

            if (pp == bishop || pp == queen)
                cnt++;

            if (pp != Piece::EMPTY)
                break;

            row += MoveDirection::BISHOP_QUEEN_DR[i];
            col += MoveDirection::BISHOP_QUEEN_DC[i];
        }
    }

    return cnt;
}

bool isDiagonalAttacking(const Board& board, Position pos, const Player player)
{
    const Piece bishop = makePiece(player, 'B');
    const Piece queen = makePiece(player, 'Q');

    for (int i = 0; i < 4; i++)
    {
        int row = pos.row + MoveDirection::BISHOP_QUEEN_DR[i];
        int col = pos.col + MoveDirection::BISHOP_QUEEN_DC[i];

        while (true)
        {
            if (!isInBoard(row, col))
                break;

            const Piece pp = board.board[row][col];

            if (pp == bishop || pp == queen)
                return true;

            if (pp != Piece::EMPTY)
                break;

            row += MoveDirection::BISHOP_QUEEN_DR[i];
            col += MoveDirection::BISHOP_QUEEN_DC[i];
        }
    }

    return false;
}

int countStraightAttacks(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    const Piece rook = makePiece(player, 'R');
    const Piece queen = makePiece(player, 'Q');

    for (int i = 0; i < 4; i++)
    {
        int row = pos.row + MoveDirection::ROOK_QUEEN_DR[i];
        int col = pos.col + MoveDirection::ROOK_QUEEN_DC[i];

        while (true)
        {
            if (!isInBoard(row, col))
                break;

            const Piece pp = board.board[row][col];

            if (pp == rook || pp == queen)
                cnt++;

            if (pp != Piece::EMPTY)
                break;

            row += MoveDirection::ROOK_QUEEN_DR[i];
            col += MoveDirection::ROOK_QUEEN_DC[i];
        }
    }

    return cnt;
}

bool isStraightAttacking(const Board& board, Position pos, const Player player)
{
    int cnt = 0;
    const Piece rook = makePiece(player, 'R');
    const Piece queen = makePiece(player, 'Q');

    for (int i = 0; i < 4; i++)
    {
        int row = pos.row + MoveDirection::ROOK_QUEEN_DR[i];
        int col = pos.col + MoveDirection::ROOK_QUEEN_DC[i];

        while (true)
        {
            if (!isInBoard(row, col))
                break;

            const Piece pp = board.board[row][col];

            if (pp == rook || pp == queen)
                return true;

            if (pp != Piece::EMPTY)
                break;

            row += MoveDirection::ROOK_QUEEN_DR[i];
            col += MoveDirection::ROOK_QUEEN_DC[i];
        }
    }

    return false;
}

// Calculate attacks in the designated square.
int countSquareAttacks(const Board& board, Position pos, const Player player)
{
    // check position.
    ENGINE_ASSERT(isInBoard(pos));

    int cnt = 0;
    cnt += countPawnAttacks(board, pos, player);
    cnt += countKnightAttacks(board, pos, player);
    cnt += countDiagonalAttacks(board, pos, player);
    cnt += countStraightAttacks(board, pos, player);
    cnt += countKingAttacks(board, pos, player);
    return cnt;
}

// Check whether the designated square is attacked.
bool isSquareAttacked(const Board& board, Position pos, const Player player)
{
    // check position
    ENGINE_ASSERT(isInBoard(pos));

    if (isPawnAttacking(board, pos, player))
        return true;
    if (isKnightAttacking(board, pos, player))
        return true;
    if (isDiagonalAttacking(board, pos, player))
        return true;
    if (isStraightAttacking(board, pos, player))
        return true;
    if (isKingAttacking(board, pos, player))
        return true;

    return false;
}
