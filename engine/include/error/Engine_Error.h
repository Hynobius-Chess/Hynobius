#pragma once
#include <stdexcept>

class EngineError : public std::runtime_error
{
public:
    explicit EngineError(const std::string& message) : std::runtime_error(message) {}
};

class FenParseError : public EngineError
{
public:
    FenParseError() : EngineError("invalid FEN") {}
    explicit FenParseError(const std::string& reason) : EngineError("invalid FEN: " + reason) {}
};

class UciCommandError : public EngineError
{
public:
    UciCommandError() : EngineError("invalid UCI command") {}
    explicit UciCommandError(const std::string& reason)
        : EngineError("invalid UCI command: " + reason)
    {
    }
};

class UciMoveParserError : public EngineError
{
public:
    UciMoveParserError() : EngineError("invalid UCI move") {}
    explicit UciMoveParserError(const std::string& reason)
        : EngineError("invalid UCI move: " + reason)
    {
    }
};