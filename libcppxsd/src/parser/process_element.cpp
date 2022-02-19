#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_element(State &state, const pugi::xml_node &node)
{
    std::cout << "element: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
