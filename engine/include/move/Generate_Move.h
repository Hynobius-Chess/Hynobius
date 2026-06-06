#pragma once

#include "Type.h"
#include "board/Board.h"

int generateAllLegalMoves(Board& board, BitMove* buffer);

int generateLegalCaptureMoves(Board& board, BitMove* buffer);