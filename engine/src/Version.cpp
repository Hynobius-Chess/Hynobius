#include "Version.h"

#include <string>
#include <string_view>

namespace Hynobius
{
constexpr std::string_view ENGINE_NAME = "Hynobius";
constexpr std::string_view ENGINE_VERSION = "0.3";
constexpr std::string_view ENGINE_AUTHOR = "Emmetthor";

std::string engineName()
{
    return std::string(ENGINE_NAME);
}

std::string engineVersion()
{
    return std::string(ENGINE_VERSION);
}

std::string engineFullName()
{
    return std::string(ENGINE_NAME) + " " + std::string(ENGINE_VERSION);
}

std::string engineAuthor()
{
    return std::string(ENGINE_AUTHOR);
}
} // namespace Hynobius