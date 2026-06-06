#pragma once

#include "Engine.h"

void uciLoop(Engine& engine);

std::string UCIMoveToString(const Move& move);

struct UCIGoLimit
{
    bool isPerft = false;
    bool isBench = false;
    bool isSpeedTest = false;
    bool isMoveTime = false;
    int64_t movetime = 0;
};
