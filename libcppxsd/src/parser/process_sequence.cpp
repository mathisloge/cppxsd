#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_sequence(State &state, const pugi::xml_node &node)
{
    std::cout << "sequence: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
