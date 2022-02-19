#pragma once
#include <unordered_set>
#include <curl/curl.h>
#include "defs.hpp"
namespace cppxsd::parser
{
struct State
{
    std::unordered_set<std::string> already_parsed;
    fs::path current_file;
};
} // namespace cppxsd::parser
