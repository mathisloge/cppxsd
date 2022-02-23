#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_sequence(State &state, const pugi::xml_node &node)
{
    for (const auto &child_node : node)
    {
        process_node(kNodeId_sequence,
                     {kNodeId_element, kNodeId_group, kNodeId_choice, kNodeId_sequence, kNodeId_any},
                     state,
                     node);
    }
}
} // namespace cppxsd::parser
