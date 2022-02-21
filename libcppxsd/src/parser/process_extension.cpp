#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_extension(State &state, const pugi::xml_node &node)
{
    const auto base_attr = require_attr(kNodeId_extension, "base", node);
    const auto content_node = node_find_child_of(node, {kNodeId_group, kNodeId_all, kNodeId_choice, kNodeId_sequence});
    if (!content_node)
    {
        state.current_el = type_str_to_element_type(base_attr.as_string());
        return;
    }

    process_node(
        kNodeId_extension, {kNodeId_group, kNodeId_all, kNodeId_choice, kNodeId_sequence}, state, content_node);
    state.current_el = meta::CustomType{"", base_attr.as_string(), false, std::move(state.current_el)};
}
} // namespace cppxsd::parser
