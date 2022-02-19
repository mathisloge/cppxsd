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
    try
    {
        process_file(state, xsd_file);
    }
    catch (const std::exception &ex)
    {
        std::cout << "FAILED: " << ex.what() << std::endl;
    }
}
} // namespace cppxsd::parser
