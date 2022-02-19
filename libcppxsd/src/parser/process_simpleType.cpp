#include "nodes.hpp"
namespace cppxsd::parser
{
void process_simpleType(State &state, const pugi::xml_node &node)
{
    const auto type_name_attr = node.attribute("name");
    if (!type_name_attr)
        throw ParseAttrException{kNodeId_simpleType, "name", node};

    const std::string type_name{type_name_attr.as_string()};
    meta::TypeDefinition type_def{type_name};
    for (const auto &cn : node)
    {
        const auto cn_name = cn.name();

        if (is_node_type(kNodeId_annotation, cn_name))
            process_annotation(state, cn);
        else if (is_node_type(kNodeId_restriction, cn_name))
        {
            PRINT_TODO_NODE(kNodeId_restriction);
        }
        else if (is_node_type(kNodeId_list, cn_name))
        {
            PRINT_TODO_NODE(kNodeId_list);
        }
        else if (is_node_type(kNodeId_union, cn_name))
        {
            PRINT_TODO_NODE(kNodeId_union);
        }
        else
            throw ParseException{kNodeId_simpleType, {kNodeId_restriction, kNodeId_list, kNodeId_union}, cn};
    }

    type_def.element = std::move(state.current_el);

    state.current_el_name = type_name;
    state.current_el = std::move(type_def);
}
} // namespace cppxsd::parser
