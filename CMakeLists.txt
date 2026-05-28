    cmake_minimum_required(VERSION 3.16)
    project(Hynobius)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_GLIBCXX_DEBUG)
        add_definitions(-DENGINE_DEBUG)
    endif()

    add_library(engine-core
        engine/src/board/Piece.cpp
        engine/src/board/Board.cpp
        engine/src/board/Attack.cpp
        engine/src/board/Check.cpp

        engine/src/debug/Perft.cpp

        engine/src/move/Move.cpp
        engine/src/move/Move_Order.cpp
        engine/src/move/Generate_Move.cpp
        engine/src/move/Make_Move.cpp
        engine/src/move/Make_BitMove.cpp

        engine/src/evaluate/Material_Point.cpp
        engine/src/evaluate/Evaluate.cpp
        engine/src/evaluate/Control_Center.cpp
        engine/src/evaluate/Tempo.cpp
        engine/src/evaluate/PST.cpp
        engine/src/evaluate/King_Safety.cpp
        engine/src/evaluate/SEE.cpp
        engine/src/evaluate/Mobility.cpp
        engine/src/evaluate/Castle_Rights.cpp
        engine/src/evaluate/Pawn_Structure.cpp

        engine/src/pgn/Pgn_Parser.cpp
        engine/src/pgn/Pgn_Transformer.cpp

        engine/src/search/TT.cpp
        engine/src/search/Zobrist.cpp
        engine/src/search/Search.cpp

        engine/src/fen/FEN_Parser.cpp

        engine/src/command/UCI.cpp
        engine/src/command/UCI_Move_Parser.cpp
        engine/src/command/CLI.cpp

        engine/src/Engine.cpp
        engine/src/Time_Management.cpp
    )

    target_include_directories(engine-core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(engine-core PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wuninitialized
            -Wmaybe-uninitialized
        )
    endif()

    add_executable(Hynobius engine/src/main.cpp)

    target_link_libraries(Hynobius PUBLIC engine-core)

    set_target_properties(Hynobius PROPERTIES WIN32_EXECUTABLE OFF)