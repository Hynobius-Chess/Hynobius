#pragma once

#include "Type.h"
#include "board/Board.h"

// Count attacks made by player's pawns towards pos
int countPawnAttacks(const Board& board, Position pos, const Player player);

// Count attacks made by player's knights towards pos
int countKnightAttacks(const Board& board, Position pos, const Player player);

// Count all attacks made by player's pieces towards pos
int countSquareAttacks(const Board& board, Position pos, const Player player);

// Returns whether the pos is attacked by player's pieces
bool isSquareAttacked(const Board& board, Position pos, const Player player);