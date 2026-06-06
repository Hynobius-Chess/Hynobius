#include "command/UCI.h"
#include "Structure_IO.h"
#include "Time_Management.h"
#include "command/UCI_Move_Parcer.h"
#include "debug/perft.h"
#include "error/Engine_Error.h"
#include "move/Move.h"
#include "pgn/Pgn_Transformer.h"
#include "search/Search.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

std::string castleMap[2][3]{{"invalid", "e1g1", "e1c1"}, {"invalid", "e8g8", "e8c8"}};

std::string UCIMoveToString(const Move& move)
{
    std::string res;
    if (move.castle == SHORT_CASTLE || move.castle == LONG_CASTLE)
    {
        res += castleMap[playerToIndex(move.player)][static_cast<int>(move.castle)];
    }
    else
    {
        res += positionToPgn(move.from) + positionToPgn(move.to);
    }

    if (move.isPromotion)
    {
        char promotionChar = '.';
        switch (pieceToChar(move.promotionPiece))
        {
            case 'Q':
            case 'q':
                promotionChar = 'q';
                break;
            case 'R':
            case 'r':
                promotionChar = 'r';
                break;
            case 'N':
            case 'n':
                promotionChar = 'n';
                break;
            case 'B':
            case 'b':
                promotionChar = 'b';
                break;
            default:
                ENGINE_FATAL("uci", "promotion piece is not valid: ", move.promotionPiece);
        }
        res += promotionChar;
    }

    return res;
}

void handleGo(std::istringstream& iss, Engine& engine)
{
    TimeManage tm;
    std::string token;
    UCIGoLimit limit;

    while (iss >> token)
    {
        if (token == "depth")
        {
            iss >> tm.depth;
        }
        if (token == "wtime")
        {
            iss >> tm.wtime;
        }
        if (token == "btime")
        {
            iss >> tm.btime;
        }
        if (token == "winc")
        {
            iss >> tm.winc;
        }
        if (token == "binc")
        {
            iss >> tm.binc;
        }
        if (token == "movetime")
        {
            iss >> limit.movetime;
            limit.isMoveTime = true;
        }
        if (token == "perft")
        {
            limit.isPerft = true;
            iss >> tm.depth;
        }
    }

    if (limit.isPerft)
    {
        if (tm.depth <= 0)
        {
            throw UciCommandError("perft command should contain a positive \'depth\' argument");
        }

        PerftStats stats = perftWithStat(engine.board, tm.depth);

        std::cout << "nodes=" << stats.nodes;
        std::cout << " captures=" << stats.captures;
        std::cout << " enPassants=" << stats.enPassants;
        std::cout << " castles=" << stats.castles;
        std::cout << " promotions=" << stats.promotions;
        std::cout << " checks=" << stats.checks << '\n' << std::flush;

        return;
    }

    BitMove move;

    if (limit.isMoveTime)
    {
        move = engine.goTime(limit.movetime);
    }

    else if (tm.depth != -1)
    {
        move = engine.goDepth(tm.depth);
    }
    else // WARN movetime is not implemented yet.
    {
        DOUT("UCI") << "start clock search\n";
        move = engine.goClock(tm);
    }

    if (move == INVALID_BITMOVE)
    {
        std::cout << "bestmove 0000\n";
        return;
    }

    std::cout << "bestmove " << UCIMoveToString(bitMovetoOriMove(engine.board, move)) << '\n';
}

void handlePosition(std::istringstream& iss, Engine& engine)
{
    std::string token, fen;
    iss >> token;

    if (token == "startpos")
    {
        engine.setStartPosition();
    }
    else if (token == "fen")
    {
        for (int t = 0; t < 6; t++)
        {
            iss >> token;
            if (!fen.empty())
                fen += " ";
            fen += token;
        }
        engine.setPositionWithFen(fen);
    }
    else
    {
        throw UciCommandError("position command should start with \'fen\' or \'startpos\'");
    }

    // no moves command
    if (!(iss >> token))
    {
        return;
    }

    if (token == "moves")
    {
        std::string strMove;
        while (iss >> strMove)
        {
            Move move = parseUCIMove(strMove, engine.board);
            engine.move(move);
            engine.board.pushRepetitionKey();
        }
    }
}

void handleBench(std::istringstream& iss, Engine& engine)
{
    std::string token;

    bool isPerft = false; // TODO
    int depth = -1;

    while (iss >> token)
    {
        if (token == "depth")
        {
            iss >> depth;
        }
    }

    if (depth <= 0)
    {
        throw UciCommandError("bench command should contain a positive \'depth\' argument");
    }

    std::vector<std::string> fen = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",             // startpos
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", // kiwipipe
        "q3k3/p1p5/2pb2r1/4ppp1/2N2r2/1Q2N2b/PPP2P1P/1R2R1K1 w - - 4 27",       // middle game
        "r4rk1/ppp2ppp/1n3q2/2Pp4/1n1P4/1P2BP1B/P4P1P/R2Q1RK1 b - - 0 15",      // middle game
        "r2q2r1/p1p1pk1p/1pn2ppb/2Nn1b2/3P4/5NB1/PPP1QPPP/1K1R1B1R w - - 0 14", // middle game
        "8/1Br2k2/4p1p1/4P1Pp/pprP1n1P/8/1P1R1P2/3R2K1 w - - 2 35",             // endgame
        "8/3p2p1/4p3/4Pk1p/2p4P/P1b5/P5P1/1R5K w - - 0 49",                     // endgame
        "4r1k1/1pR2pp1/7p/1P2rP2/P4K1P/3R2P1/8/8 b - - 0 43",                   // endgame
    };

    SearchStats totalStats;
    auto startTime = std::chrono::steady_clock::now();
    int64_t totalNodes = 0;

    for (int i = 0; i < (int)fen.size(); i++)
    {
        engine.newGame();
        engine.setPositionWithFen(fen[i]);
        SearchResult result = engine.fullInfoSearch(depth);
        result.stats.print();
        totalNodes += result.stats.totalNodes();
    }

    auto endTime = std::chrono::steady_clock::now();
    int64_t time =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "\n---------\n\n";
    std::cout << "totalnodes=" << totalNodes;
    std::cout << " timeMs=" << time;
    std::cout << " nps=" << (time > 0 ? totalNodes * 1000 / time : 0);
    std::cout << '\n' << std::flush;
}

void uciLoop(Engine& engine)
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        try
        {
            std::istringstream iss(line);
            std::string token;
            iss >> token;

            if (token == "uci")
            {
                std::cout << "id name Hynobius 0.3\nid author EmmetThor\n";
                std::cout << "uciok\n" << std::flush;
            }
            else if (token == "isready")
            {
                std::cout << "readyok\n" << std::flush;
            }
            else if (token == "position")
            {
                handlePosition(iss, engine);
            }
            else if (token == "go")
            {
                handleGo(iss, engine);
            }
            else if (token == "bench")
            {
                handleBench(iss, engine);
            }
            else if (token == "quit" || token == "stop")
            {
                break;
            }
            else if (token == "d")
            {
                std::cout << engine.board << '\n';
            }
            else if (token == "ucinewgame")
            {
                engine.newGame();
            }
            else
            {
                std::cerr << "Unrecognized token.\n" << std::flush;
            }
        }
        catch (const EngineError& e)
        {
            std::cout << "info string error: " << e.what() << std::endl;
        }
    }
}