#include "parser.hpp"
#include <iostream>
#include <curl/easy.h>
#include <pugixml.hpp>
#include "nodes.hpp"

namespace cppxsd::parser
{
Parser::Parser()
{}
Parser::~Parser()
{}
void Parser::addFile(const fs::path &xsd_file)
{
    process_file(state, xsd_file);
}
} // namespace cppxsd::parser
