#include "evaluate/Evaluate.h"

int Evaluate::evaluateBoard(const Board& board, EVALUATE_MODE m) const
{
    int res = 0;
    switch (m)
    {
        case EVALUATE_MODE::FULL:
            // res += evaluateKingSafety(board, Player::WHITE, evalWeight.kingSafetyWeight);
            // res -= evaluateKingSafety(board, Player::BLACK, evalWeight.kingSafetyWeight);

            // res += evaluateKnightMobility(board, Player::WHITE, evalWeight.knightMobilityWeight);
            // res -= evaluateKnightMobility(board, Player::BLACK, evalWeight.knightMobilityWeight);
            // res += evaluateBishopMobility(board, Player::WHITE, evalWeight.bishopMobilityWeight);
            // res -= evaluateBishopMobility(board, Player::BLACK, evalWeight.bishopMobilityWeight);
            // res += evaluateRookMobility(board, Player::WHITE, evalWeight.rookMobilityWeight);
            // res -= evaluateRookMobility(board, Player::BLACK, evalWeight.rookMobilityWeight);

            // res += evaluateCastleRights(board, evalWeight.castleWeight);

            res += board.materialScore;
            res += board.PSTScore;

            // res += evaluatePawnStructure(board,
            //                              evalWeight.doublePawnWeight,
            //                              evalWeight.isolatedPawnWeight,
            //                              evalWeight.passPawnWeight,
            //                              evalWeight.passPawnRankWeight);
            // res += evaluateTempo(board, board.player, evalWeight.tempo);

            break;
        default:
            res = 0;
    }

    return res;
}

int boardEvaluate()
{
    int res = 0;

    return res;
}