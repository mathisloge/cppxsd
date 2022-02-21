#include "nodes.hpp"
namespace cppxsd::parser
{
void process_element(State &state, const pugi::xml_node &node)
{
    const auto type_name_attr = require_attr(kNodeId_element, "name", node);
    const std::string type_name{type_name_attr.as_string()};

    const auto type_attr = node.attribute("type");
    if (!type_attr)
    {

        auto inline_type_node = node_find_child_of(node, {kNodeId_simpleType, kNodeId_complexType});
        process_node(kNodeId_element, {kNodeId_simpleType, kNodeId_complexType}, state, inline_type_node);

        state.current_el = meta::CustomType{type_name, {}, false, std::move(state.current_el)};
        state.current_el_name = type_name;
    }
    else
    {
        state.current_el_name = type_name;
        state.current_el = meta::TypeRef{type_name, std::string{type_attr.as_string()}};
    }
}
} // namespace cppxsd::parser
