#pragma once
#include <stdexcept>

class EngineError : public std::runtime_error
{
public:
    explicit EngineError(const std::string& message)
        : std::runtime_error(message) {}
};

class FenParseError : public EngineError
{
public:
    FenParseError()
        : EngineError("invalid FEN") {}
    explicit FenParseError(const std::string& reason)
        : EngineError("invalid FEN: " + reason) {}
};