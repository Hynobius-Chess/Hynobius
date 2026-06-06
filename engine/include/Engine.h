#pragma once

#include "Time_Management.h"
#include "board/Board.h"
#include "evaluate/Evaluate.h"
#include "move/Move.h"
#include "search/Search.h"

class Engine
{
public:
    Engine();

    Evaluate eval;
    Board board;

    void setStartPosition();
    void setPositionWithFen(const std::string& fen);
    void checkReady() const;
    void newGame();

    BitMove goDepth(int depth, bool isPrintInfo = 1);
    BitMove goClock(const TimeManage& tm);
    BitMove goTime(const int64_t time);
    SearchResult fullInfoSearch(int depth);
    void move(Move move);
    void quit() const;
    void changePlayer();
    void setPlayer(Player player);
};