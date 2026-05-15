#include "Engine.h"
#include "command/CLI.h"
#include "command/UCI.h"

int main(int argc, char* argv[])
{
    Engine engine;

    if (argc > 1)
    {
        // argv CLI
        Config cfg = parseArgs(argc, argv);
        return runCommand(cfg);
    }
    else
    {
        // loop UCI
        Engine engine;
        uciLoop(engine);
    }

    return 0;
}