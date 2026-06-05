#pragma once

#include "board/Board.h"
#include <stdexcept>
#include <string>

class FenParseError : public std::runtime_error
{
public:
    explicit FenParseError(const std::string& reason) : std::runtime_error("invalid FEN: " + reason) {}
};

Board tryCinFenToBoard(const std::string& fen);