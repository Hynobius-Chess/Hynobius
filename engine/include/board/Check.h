#pragma once

#include "Type.h"
#include "board/Board.h"

// Check whether the player is checked by the opponent
bool isInCheck(const Board& board, const Player player);