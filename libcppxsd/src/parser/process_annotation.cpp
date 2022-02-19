#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_annotation(State &state, const pugi::xml_node &node)
{
    std::cout << "annotation: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
