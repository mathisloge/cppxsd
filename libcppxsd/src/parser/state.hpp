#pragma once
#include <unordered_set>
#include <curl/curl.h>
#include "../meta/types.hpp"
#include "defs.hpp"
namespace cppxsd::parser
{
struct State
{
    std::unordered_set<std::string> already_parsed;
    fs::path current_file;

    meta::GlobalNamespace global;
    meta::ElementType current_el;
    meta::VarName current_el_name;
};
} // namespace cppxsd::parser
