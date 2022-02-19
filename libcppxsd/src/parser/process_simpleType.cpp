#include <iostream>
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_simpleType(State &state, const pugi::xml_node &node)
{
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
}
} // namespace cppxsd::parser
