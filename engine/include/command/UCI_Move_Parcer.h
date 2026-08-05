#pragma once

#include "move/Move.h"

// Turng UCI move (string) to `Move`
Move parseUCIMove(const std::string strMove, const Board& board);