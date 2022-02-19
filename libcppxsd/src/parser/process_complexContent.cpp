#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_complexContent(State &state, const pugi::xml_node &node)
{
    std::cout << "complexContent: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
