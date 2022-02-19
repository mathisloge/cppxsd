#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_group(State &state, const pugi::xml_node &node)
{
    std::cout << "group: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
