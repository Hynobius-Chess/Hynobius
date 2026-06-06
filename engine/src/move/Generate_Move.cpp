#include "Type.h"
#include "board/Attack.h"
#include "board/Board.h"
#include "board/Check.h"
#include "board/Piece.h"
#include "debug/log.h"
#include "debug/validation.h"
#include "move/Generate_Position.h"
#include "move/Generate_Variables.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#pragma GCC optimize("O3,unroll-loops")

// WARN must be sure that doBitMove and undoBitMove won't break board states.
bool isPseudoLegalMoveLegal(Board& board, const BitMove move)
{
    UndoState undo;

    doBitMove(board, move, undo);

    const Player movedPlayer = opponent(board.player);
    const bool legal = !isInCheck(board, movedPlayer);

    undoBitMove(board, move, undo);

    return legal;
}

template <typename Emit>
void generatePieceMoves(const Board& board, const Piece movePiece, Emit&& emit)
{
    // check inputs.
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    const Player player = board.player;
    const int pieceCount = board.getPieceCount(movePiece);
    const auto* posArray = board.getPiecePos(movePiece);

    // WARN creating posBuffer might be slow.
    Position posBuffer[GenearteVarialble::MAX_ONE_PIECE_MOVES];

    int cnt = 0;
    for (int i = 0; i < pieceCount; i++)
    {
        const Square fromSquare = positionToSquare(posArray[i]);

        generatePiecePosFromPos(board,
                                posArray[i],
                                movePiece,
                                [&](const Position toPos)
                                {
                                    const BitMove move =
                                        makeBitMove(fromSquare,
                                                    positionToSquare(toPos),
                                                    Piece::EMPTY,
                                                    board.at(toPos) != Piece::EMPTY,
                                                    false,
                                                    false,
                                                    false);

                                    emit(move);
                                });
    }
}

template <typename Emit>
void generatePieceCapture(const Board& board, const Piece movePiece, Emit&& emit)
{
    // check inputs.
    ENGINE_ASSERT(movePiece != Piece::EMPTY);

    const Player player = board.player;
    const int pieceCount = board.getPieceCount(movePiece);
    const auto* posArray = board.getPiecePos(movePiece);

    // WARN creating posBuffer might be slow.
    Position posBuffer[GenearteVarialble::MAX_ONE_PIECE_MOVES];

    int cnt = 0;
    for (int i = 0; i < pieceCount; i++)
    {
        const Square fromSquare = positionToSquare(posArray[i]);

        generatePiecePosFromPos(board,
                                posArray[i],
                                movePiece,
                                [&](const Position toPos)
                                {
                                    const bool isCapture = board.at(toPos) != Piece::EMPTY;
                                    if (!isCapture)
                                        return;

                                    const BitMove move = makeBitMove(fromSquare,
                                                                     positionToSquare(toPos),
                                                                     Piece::EMPTY,
                                                                     isCapture,
                                                                     false,
                                                                     false,
                                                                     false);

                                    emit(move);
                                });
    }
}

template <typename Emit> int generatePawnQuietMoves(const Board& board, Emit&& emit)
{
    const Player player = board.player;

    int cnt = 0;
    const Piece pawn = makePiece(player, 'P');

    const int dr = (player == Player::WHITE ? -1 : 1);
    const int startRank = (player == Player::WHITE ? 6 : 1);
    const int promoteRank = (player == Player::WHITE ? 0 : 7);
    const int pawnCount = board.getPieceCount(pawn);
    const auto* posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++)
    {
        const auto [r, c] = posArray[i];
        const Position fromPos = posArray[i];
        const Position pushOneStep = {r + dr, c};
        const Position pushTwoStep = {r + 2 * dr, c};
        const Square fromSquare = positionToSquare(fromPos);
        const Square pushOneStepSquare = positionToSquare(pushOneStep);
        const Square pushTwoStepSquare = positionToSquare(pushTwoStep);

        if (isInBoard(pushOneStep) && board.at(pushOneStep) == Piece::EMPTY)
        {
            if (pushOneStep.row == promoteRank)
            {
                const Piece knight = makePiece(player, 'N');
                const Piece bishop = makePiece(player, 'B');
                const Piece rook = makePiece(player, 'R');
                const Piece queen = makePiece(player, 'Q');

                for (auto promo : {knight, bishop, rook, queen})
                {
                    const BitMove move = makeBitMove(
                        fromSquare, pushOneStepSquare, promo, false, false, false, true);

                    emit(move);
                }
            }
            else
            {
                const BitMove move = makeBitMove(
                    fromSquare, pushOneStepSquare, Piece::EMPTY, false, false, false, false);

                emit(move);
            }
        }

        if (r == startRank && board.at(pushOneStep) == Piece::EMPTY &&
            board.at(pushTwoStep) == Piece::EMPTY)
        {
            const BitMove move = makeBitMove(
                fromSquare, pushTwoStepSquare, Piece::EMPTY, false, false, false, false);

            emit(move);
        }
    }

    return cnt;
}

template <typename Emit> int generatePawnCaptures(const Board& board, Emit&& emit)
{
    int cnt = 0;
    const Player player = board.player;
    const Piece pawn = makePiece(player, 'P');

    const int dr = (player == Player::WHITE ? -1 : 1);
    const int startRank = (player == Player::WHITE ? 6 : 1);
    const int promoteRank = (player == Player::WHITE ? 0 : 7);
    const int pawnCount = board.getPieceCount(pawn);
    const auto* posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++)
    {
        const auto [r, c] = posArray[i];
        const Position fromPos = posArray[i];
        const Square fromSquare = positionToSquare(fromPos);

        for (auto dc : {-1, 1})
        {
            const Position toPos = {r + dr, c + dc};
            if (!isInBoard(toPos))
                continue;
            if (board.at(toPos) == Piece::EMPTY)
                continue;
            if (isSameColor(board.at(toPos), pawn))
                continue;

            const Square toSquare = positionToSquare(toPos);

            if (toPos.row == promoteRank)
            {
                const Piece knight = makePiece(player, 'N');
                const Piece bishop = makePiece(player, 'B');
                const Piece rook = makePiece(player, 'R');
                const Piece queen = makePiece(player, 'Q');

                for (auto promo : {knight, bishop, rook, queen})
                {
                    const BitMove move =
                        makeBitMove(fromSquare, toSquare, promo, true, false, false, true);

                    emit(move);
                }
            }
            else
            {
                const BitMove move =
                    makeBitMove(fromSquare, toSquare, Piece::EMPTY, true, false, false, false);

                emit(move);
            }
        }
    }

    return cnt;
}

template <typename Emit> int generateEnPassants(const Board& board, Emit&& emit)
{
    // if it is not possible to en passant
    if (board.enPassantPos == POS_NONE)
        return 0;

    int cnt = 0;
    const Player player = board.player;
    const Piece pawn = makePiece(player, 'P');

    const int dr = (player == Player::WHITE ? -1 : 1);
    const int startRank = (player == Player::WHITE ? 6 : 1);
    const int promoteRank = (player == Player::WHITE ? 0 : 7);
    const int pawnCount = board.getPieceCount(pawn);
    const auto* posArray = board.getPiecePos(pawn);

    for (int i = 0; i < pawnCount; i++)
    {
        const auto [r, c] = posArray[i];
        const Position fromPos = posArray[i];
        const Square fromSquare = positionToSquare(fromPos);

        for (auto dc : {-1, 1})
        {
            const Position toPos = {r + dr, c + dc};
            if (!isInBoard(toPos))
                continue;

            if (toPos != board.enPassantPos)
                continue;

            const Position capturePos = {fromPos.row, toPos.col};
            if (board.at(capturePos) != makePiece(opponent(player), 'P'))
                continue;

            const Square toSquare = positionToSquare(toPos);

            const BitMove move =
                makeBitMove(fromSquare, toSquare, Piece::EMPTY, true, false, true, false);

            emit(move);
        }
    }

    return cnt;
}

template <typename Emit> int generateCastling(const Board& board, Emit&& emit)
{
    // fast check.
    if (board.castleRights == 0)
        return 0;

    int cnt = 0;
    const Player player = board.player;

    const int row = (player == Player::WHITE ? 7 : 0);

    const Position kingPos = {row, 4};
    const Square kingSquare = positionToSquare(kingPos);

    // safety: king must be on e-file.
    if (board.at(kingPos) != makePiece(player, 'K'))
    {
        return 0;
    }

    // king side.
    if (player == Player::WHITE)
    {
        // white king side.
        if (board.castleRights & 0b0100)
        {
            const Position f = {row, 5};
            const Position g = {row, 6};
            const Position h = {row, 7};
            if (board.at(h) == makePiece(board.player, 'R') && board.at(f) == Piece::EMPTY &&
                board.at(g) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::BLACK) &&
                    !isSquareAttacked(board, f, Player::BLACK) &&
                    !isSquareAttacked(board, g, Player::BLACK))
                {
                    const BitMove move = makeBitMove(
                        kingSquare, positionToSquare(g), Piece::EMPTY, false, true, false, false);

                    emit(move);
                }
            }
        }
    }
    else
    {
        // black king side.
        if (board.castleRights & 0b0001)
        {
            const Position f = {row, 5};
            const Position g = {row, 6};
            const Position h = {row, 7};

            if (board.at(h) == makePiece(board.player, 'R') && board.at(f) == Piece::EMPTY &&
                board.at(g) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::WHITE) &&
                    !isSquareAttacked(board, f, Player::WHITE) &&
                    !isSquareAttacked(board, g, Player::WHITE))
                {
                    const BitMove move = makeBitMove(
                        kingSquare, positionToSquare(g), Piece::EMPTY, false, true, false, false);

                    emit(move);
                }
            }
        }
    }

    // queen side
    if (player == Player::WHITE)
    {
        // white queen side.
        if (board.castleRights & 0b1000)
        {
            const Position a = {row, 0};
            const Position b = {row, 1};
            const Position c = {row, 2};
            const Position d = {row, 3};

            if (board.at(a) == makePiece(board.player, 'R') && board.at(d) == Piece::EMPTY &&
                board.at(c) == Piece::EMPTY && board.at(b) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::BLACK) &&
                    !isSquareAttacked(board, d, Player::BLACK) &&
                    !isSquareAttacked(board, c, Player::BLACK))
                {
                    const BitMove move = makeBitMove(
                        kingSquare, positionToSquare(c), Piece::EMPTY, false, true, false, false);

                    emit(move);
                }
            }
        }
    }
    else
    {
        // black queen side.
        if (board.castleRights & 0b0010)
        {
            const Position a = {row, 0};
            const Position b = {row, 1};
            const Position c = {row, 2};
            const Position d = {row, 3};

            if (board.at(a) == makePiece(board.player, 'R') && board.at(d) == Piece::EMPTY &&
                board.at(c) == Piece::EMPTY && board.at(b) == Piece::EMPTY)
            {
                if (!isSquareAttacked(board, kingPos, Player::WHITE) &&
                    !isSquareAttacked(board, d, Player::WHITE) &&
                    !isSquareAttacked(board, c, Player::WHITE))
                {
                    const BitMove move = makeBitMove(
                        kingSquare, positionToSquare(c), Piece::EMPTY, false, true, false, false);

                    emit(move);
                }
            }
        }
    }
    return cnt;
}

template <typename Emit>
int generatePseudoLegalMoves(const Board& board, BitMove* buffer, Emit&& emit)
{
    const Player player = board.player;

    // checks.
    checkBoardState(board);

    int cnt = 0;
    const Piece pawn = makePiece(player, 'P'), knight = makePiece(player, 'N'),
                bishop = makePiece(player, 'B'), rook = makePiece(player, 'R'),
                queen = makePiece(player, 'Q'), king = makePiece(player, 'K');

    generatePieceMoves(board, knight, emit);
    generatePieceMoves(board, bishop, emit);
    generatePieceMoves(board, rook, emit);
    generatePieceMoves(board, queen, emit);
    generatePieceMoves(board, king, emit);

    generatePawnQuietMoves(board, emit);
    generatePawnCaptures(board, emit);

    generateCastling(board, emit);
    generateEnPassants(board, emit);

    return cnt;
}

template <typename Emit>
int generatePseudoLegalCaptures(const Board& board, BitMove* buffer, Emit&& emit)
{
    const Player player = board.player;

    checkBoardState(board);

    int cnt = 0;
    const Piece pawn = makePiece(player, 'P'), knight = makePiece(player, 'N'),
                bishop = makePiece(player, 'B'), rook = makePiece(player, 'R'),
                queen = makePiece(player, 'Q'), king = makePiece(player, 'K');

    generatePieceCapture(board, knight, emit);
    generatePieceCapture(board, bishop, emit);
    generatePieceCapture(board, rook, emit);
    generatePieceCapture(board, queen, emit);
    generatePieceCapture(board, king, emit);

    generatePawnCaptures(board, emit);
    generateEnPassants(board, emit);

    return cnt;
}

int filterLegalMoves(const Board& board, BitMove* allMoves, int nAllMoves, BitMove* buffer)
{
    int cnt = 0;
    Board copyBoard = board;
    UndoState undo;

    checkBoardState(board);

    for (int i = 0; i < nAllMoves; i++)
    {
        BitMove move = allMoves[i];

        doBitMove(copyBoard, move, undo);

        if (!isInCheck(copyBoard, opponent(copyBoard.player)))
        {
            buffer[cnt++] = move;
        }

        undoBitMove(copyBoard, move, undo);
    }

    return cnt;
}

int generateAllLegalMoves(Board& board, BitMove* buffer)
{
    ENGINE_ASSERT(isPlayerValid(board.player));

    int cnt = 0;
    generatePseudoLegalMoves(board,
                             buffer,
                             [&](BitMove move)
                             {
                                 if (isPseudoLegalMoveLegal(board, move))
                                 {
                                     buffer[cnt++] = move;
                                 }
                             });

    return cnt;
}

int generateLegalCaptureMoves(Board& board, BitMove* buffer)
{
    ENGINE_ASSERT(isPlayerValid(board.player));

    int cnt = 0;
    generatePseudoLegalCaptures(board,
                                buffer,
                                [&](BitMove move)
                                {
                                    if (isPseudoLegalMoveLegal(board, move))
                                    {
                                        buffer[cnt++] = move;
                                    }
                                });

    return cnt;
}