#include "nodes.hpp"
namespace cppxsd::parser
{
void process_complexContent(State &state, const pugi::xml_node &node)
{
    const auto content_node = node_find_child_of(node, {kNodeId_restriction, kNodeId_extension});
    process_node(kNodeId_complexType, {kNodeId_restriction, kNodeId_extension}, state, content_node);
}
} // namespace cppxsd::parser
