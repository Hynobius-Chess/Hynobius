#pragma once

#include "board/Board.h"

#include <string>

extern const char colToPgn[8];
extern const char rowToPgn[8];

Position pgnToPosition(const std::string& s);

std::string positionToPgn(const Position pos);