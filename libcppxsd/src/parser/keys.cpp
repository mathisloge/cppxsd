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
        {kNodeId_union, NodeType::xsd_union},
        {kNodeId_extension, NodeType::extension}};
    for (const auto &e : lookup)
        if (is_node_type(e.first, str))
            return e.second;
    return NodeType::unknown;
}

//! https://www.w3.org/TR/xmlschema-2/
//! https://www.w3.org/TR/xmlschema-2/type-hierarchy.gif
BuildinType value_name_to_type(const std::string_view str)
{
    static const std::unordered_map<std::string_view, BuildinType> lookup{
        {kBuildinTypeId_duration, BuildinType::xsd_duration},
        {kBuildinTypeId_dateTime, BuildinType::xsd_dateTime},
        {kBuildinTypeId_time, BuildinType::xsd_duration},
        {kBuildinTypeId_date, BuildinType::xsd_date},
        //! TODO: xsd_gYearMonth,
        //! TODO: xsd_gYear,
        //! TODO: xsd_gMonthDay,
        //! TODO: xsd_gDay,
        //! TODO: xsd_gMonth,
        {kBuildinTypeId_string, BuildinType::xsd_string},
        {kBuildinTypeId_boolean, BuildinType::xsd_boolean},
        {kBuildinTypeId_base64Binary, BuildinType::xsd_base64Binary},
        //! TODO: {kBuildinTypeId_hexBinary , BuildinType::xsd_hexBinary },
        {kBuildinTypeId_float, BuildinType::xsd_float},
        {kBuildinTypeId_decimal, BuildinType::xsd_decimal},
        {kBuildinTypeId_double, BuildinType::xsd_double},
        {kBuildinTypeId_anyUri, BuildinType::xsd_anyUri},
        {kBuildinTypeId_integer, BuildinType::xsd_integer},
        {kBuildinTypeId_nonPositiveInteger, BuildinType::xsd_nonPositiveInteger},
        {kBuildinTypeId_negativeInteger, BuildinType::xsd_negativeInteger},
        {kBuildinTypeId_long, BuildinType::xsd_long},
        {kBuildinTypeId_int, BuildinType::xsd_int},
        {kBuildinTypeId_short, BuildinType::xsd_short},
        {kBuildinTypeId_byte, BuildinType::xsd_byte},
        {kBuildinTypeId_nonNegativeInteger, BuildinType::xsd_nonNegativeInteger},
        {kBuildinTypeId_unsignedLong, BuildinType::xsd_unsignedLong},
        {kBuildinTypeId_unsignedInt, BuildinType::xsd_unsignedInt},
        {kBuildinTypeId_unsignedShort, BuildinType::xsd_unsignedShort},
        {kBuildinTypeId_unsignedByte, BuildinType::xsd_unsignedByte},
        {kBuildinTypeId_positiveInteger, BuildinType::xsd_positiveInteger},
        {kBuildinTypeId_normalizedString, BuildinType::xsd_normalizedString}
        //! TODO: {kBuildinTypeId_token , BuildinType::xsd_token },
        //! TODO: {kBuildinTypeId_language , BuildinType::xsd_language },
    };

    for (const auto &e : lookup)
        if (is_node_type(e.first, str))
            return e.second;
    return BuildinType::unknown;
}
} // namespace cppxsd::parser
