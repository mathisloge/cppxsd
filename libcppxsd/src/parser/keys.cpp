#include "keys.hpp"
#include <unordered_map>
#include "helpers.hpp"

namespace cppxsd::parser
{
NodeType node_name_to_type(const std::string_view str)
{
    static const std::unordered_map<std::string_view, NodeType> lookup{
        {kNodeId_schema, NodeType::schema},
        {kNodeId_include, NodeType::include},
        {kNodeId_import, NodeType::import},
        {kNodeId_redefine, NodeType::redefine},
        {kNodeId_simpleType, NodeType::simpleType},
        {kNodeId_complexType, NodeType::complexType},
        {kNodeId_group, NodeType::group},
        {kNodeId_attribute, NodeType::attribute},
        {kNodeId_attributeGroup, NodeType::attributeGroup},
        {kNodeId_element, NodeType::element},
        {kNodeId_notation, NodeType::notation},
        {kNodeId_annotation, NodeType::annotation},
        {kNodeId_restriction, NodeType::restriction},
        {kNodeId_list, NodeType::list},
        {kNodeId_union, NodeType::xsd_union}};
    for (const auto &e : lookup)
        if (is_node_type(e.first, str))
            return e.second;
    return NodeType::unknown;
}

BuildinType value_name_to_type(const std::string_view str)
{
    static const std::unordered_map<std::string_view, BuildinType> lookup{
        {kBuildinTypeId_string, BuildinType::xsd_string}};

    for (const auto &e : lookup)
        if (is_node_type(e.first, str))
            return e.second;
    return BuildinType::unknown;
}
} // namespace cppxsd::parser
