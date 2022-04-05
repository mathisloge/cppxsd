#pragma once
#include <unordered_set>
#include <cppxsd/ast.hpp>
#include <curl/curl.h>
#include "defs.hpp"
namespace cppxsd::parser
{
struct State
{
    std::unordered_set<std::string> already_parsed;
    fs::path current_file;
    std::vector<std::shared_ptr<meta::schema>> schemas;
};
} // namespace cppxsd::parser
