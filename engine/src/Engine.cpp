#include "Engine.h"

#include "Time_Management.h"
#include "board/Board.h"
#include "board/Piece.h"
#include "debug/validation.h"
#include "fen/FEN_Parser.h"
#include "move/Make_BitMove.h"
#include "move/Move.h"
#include "search/Search.h"
#include "search/Search_Variables.h"
#include "search/TT.h"
#include "search/Zobrist.h"

Engine::Engine()
{
    initZobrist();
    board.init();
    checkBoardState(board);
}

void Engine::setStartPosition()
{
    board.init();
}

void Engine::setPositionWithFen(const std::string& fen)
{
    board = tryCinFenToBoard(fen);
}

void Engine::checkReady() const {}

void Engine::move(Move move)
{
    BitMove bitMove =
        makeBitMove(positionToSquare(move.from),
                    positionToSquare(move.to),
                    move.promotionPiece,
                    (isValidPieceIndex(pieceToIndex(move.capturePiece)) ? true : false),
                    (move.castle == Castle::NOT ? false : true),
                    move.isEnPassant,
                    move.isPromotion);

    UndoState undo;
    doBitMove(board, bitMove, undo);
}

void Engine::changePlayer()
{
    board.player = opponent(board.player);
}

void Engine::setPlayer(Player player)
{
    board.player = player;
}

BitMove Engine::goDepth(int depth, bool isPrintInfo)
{
    Search search(eval, {depth, MAX_THINK_TIME});
    auto res = search.findBestMove(board);

    return res.bestBitMove;
}

BitMove Engine::goClock(const TimeManage& tm)
{
    Search search(eval, timeManager(tm, board.player));

    auto res = search.findBestMove(board);

    return res.bestBitMove;
}

BitMove Engine::goTime(const int64_t time)
{
    Search search(eval, {SearchVarialble::MAX_SEARCH_DEPTH, time});

    auto res = search.findBestMove(board);

    return res.bestBitMove;
}

SearchResult Engine::fullInfoSearch(int depth)
{
    Search search(eval, {depth, MAX_THINK_TIME});
    auto res = search.findBestMove(board);

    return res;
}

void Engine::quit() const {}

void Engine::newGame()
{
    board.init();
    for (int i = 0; i < TT_SIZE; i++)
    {
        TT[i] = {};
    }
}