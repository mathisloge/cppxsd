#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_complexType(State &state, const pugi::xml_node &node)
{
    for (const auto &n : node)
    {
        std::cout << "complexType: UNHANDLED " << n.name() << std::endl;
    }
}
} // namespace cppxsd::parser
