#pragma once
#include <cstdint>

#include "board/Board.h"

constexpr int PIECE_NB = 13; // EMPTY = 0;
constexpr int SQUARE_NB = 64;

extern uint64_t zobPiece[PIECE_NB][SQUARE_NB];
extern uint64_t zobPlayer;
extern uint64_t zobCastle[16];
extern uint64_t zobEnPassant[8];

void initZobrist();
uint64_t computeZobrist(const Board& board);
int zobBoardPosition(const Position pos);