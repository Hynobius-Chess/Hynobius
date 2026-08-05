#pragma once

#include "Engine.h"

// Go into UCI loop
void uciLoop(Engine& engine);

// Turn `Move` into UCI move (string)
std::string moveToUCIMove(const Move& move);

struct UCIGoLimit
{
    bool isPerft = false;
    bool isBench = false;
    bool isSpeedTest = false;
    bool isMoveTime = false;
    int64_t movetime = 0;
};
