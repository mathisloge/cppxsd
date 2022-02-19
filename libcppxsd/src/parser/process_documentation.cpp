#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_documentation(State &state, const pugi::xml_node &node)
{
    std::cout << "documentation: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
