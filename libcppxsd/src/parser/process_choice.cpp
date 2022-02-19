#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_choice(State &state, const pugi::xml_node &node)
{
    std::cout << "choice: UNHANDLED " << node.name() << std::endl;
}
} // namespace cppxsd::parser
