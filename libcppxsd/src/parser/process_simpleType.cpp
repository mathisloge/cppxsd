#include "nodes.hpp"
namespace cppxsd::parser
{
void process_simpleType(State &state, const pugi::xml_node &node)
{
    const auto type_name_attr = require_attr(kNodeId_simpleType, "name", node);
    const std::string type_name{type_name_attr.as_string()};
    meta::TypeDefinition type_def{type_name};
    for (const auto &cn : node)
    {
        const auto cn_name = cn.name();
        if (is_node_type(kNodeId_annotation, cn_name))
        {
            process_annotation(state, cn);
        }
        else
        {
            process_node(
                kNodeId_simpleType, {kNodeId_annotation, kNodeId_restriction, kNodeId_list, kNodeId_union}, state, cn);
            type_def.element = std::move(state.current_el);
        }
    }
    state.current_el_name = type_name;
    state.current_el = std::move(type_def);
}
} // namespace cppxsd::parser
