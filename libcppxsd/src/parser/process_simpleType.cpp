#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_simpleType(State &state, const pugi::xml_node &node)
{
    std::cout << "simpleType: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
