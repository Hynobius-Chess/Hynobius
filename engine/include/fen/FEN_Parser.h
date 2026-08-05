#pragma once

#include "board/Board.h"
#include <string>

// Turn FEN into `Board`
Board fenToBoard(const std::string& fen);