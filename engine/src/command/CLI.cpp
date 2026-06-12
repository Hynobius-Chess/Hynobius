#include "command/CLI.h"
#include "Engine.h"
#include "board/Board.h"
#include "debug/perft.h"
#include "fen/FEN_Parser.h"
#include "move/Move.h"

#define END '\n' << std::flush

#include <stdexcept>
#include <string>

Config parseArgs(int argc, char* argv[])
{
    Config cfg;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--fen")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("--fen requires a value");
            cfg.fen = argv[++i];
        }
        else if (arg == "--perft")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("--perft requires a depth");
            cfg.depth = std::stoi(argv[++i]);
            cfg.mode = RunMode::PERFT;
        }
        else if (arg == "--divide")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("--divide requires a depth");
            cfg.depth = std::stoi(argv[++i]);
            cfg.mode = RunMode::PERFT_DIVIDE;
        }
        else if (arg == "--stats")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("--stats requires a depth");
            cfg.depth = std::stoi(argv[++i]);
            cfg.mode = RunMode::PERFT_STATS;
        }
        else if (arg == "--search-with-info")
        {
            if (i + 1 >= argc)
                throw std::runtime_error("--search-with-info requires a depth");
            cfg.depth = std::stoi(argv[++i]);
            cfg.mode = RunMode::SEARCH_WITH_INFO;
        }
        else
        {
            throw std::runtime_error("unknown argument: " + arg);
        }
    }

    if (cfg.mode == RunMode::NONE)
        throw std::runtime_error("no mode specified. use --perft / --divide / --stats");

    if (cfg.depth < 0)
        throw std::runtime_error("depth must be >= 0");

    return cfg;
}

int runCommand(const Config& cfg)
{
    switch (cfg.mode)
    {
        case RunMode::PERFT:
        {
            Board board = fenToBoard(cfg.fen);
            std::cout << "nodes=" << perft(board, cfg.depth) << END;

            return 0;
        }
        case RunMode::PERFT_STATS:
        {
            Board board = fenToBoard(cfg.fen);
            PerftStats stats = perftWithStat(board, cfg.depth);

            std::cout << "nodes=" << stats.nodes << END;
            std::cout << "captures=" << stats.captures << END;
            std::cout << "enPassants=" << stats.enPassants << END;
            std::cout << "castles=" << stats.castles << END;
            std::cout << "promotions=" << stats.promotions << END;
            std::cout << "checks=" << stats.checks << END;

            return 0;
        }
        case RunMode::PERFT_DIVIDE:
        {
            std::cout << "preft divide commands is not implemented yet. Please try other commands." << END;
            return 0;
        }
        case RunMode::SEARCH_WITH_INFO:
        {
            Engine engine;
            engine.setPositionWithFen(cfg.fen);

            SearchResult result = engine.fullInfoSearch(cfg.depth);

            std::cout << "bestmove=" << bitMoveToUCIMove(result.bestBitMove) << END;
            std::cout << "score=" << result.bestScore << END;

            return 0;
        }
        default:
            throw std::runtime_error("unknown command");
    }

    return 0;
}