#pragma once

// Constexpr of piece move directions
namespace MoveDirection
{
constexpr int KNIGHT_DR[] = {1, 2, 2, 1, -1, -2, -2, -1};
constexpr int KNIGHT_DC[] = {2, 1, -1, -2, -2, -1, 1, 2};
constexpr int KING_DR[] = {1, 1, 1, 0, -1, -1, -1, 0};
constexpr int KING_DC[] = {1, 0, -1, -1, -1, 0, 1, 1};
constexpr int ROOK_QUEEN_DR[] = {1, -1, 0, 0};
constexpr int ROOK_QUEEN_DC[] = {0, 0, 1, -1};
constexpr int BISHOP_QUEEN_DR[] = {1, 1, -1, -1};
constexpr int BISHOP_QUEEN_DC[] = {1, -1, 1, -1};
} // namespace MoveDirection