#pragma once
#include <string_view>
#include <pugixml.hpp>
#include "../meta/types.hpp"
#include "defs.hpp"
#include "state.hpp"
namespace cppxsd::parser
{
inline constexpr std::string_view kNodeId_schema{"schema"};
inline constexpr std::string_view kNodeId_include{"include"};
inline constexpr std::string_view kNodeId_import{"import"}; // can be handled as kNodeId_include
inline constexpr std::string_view kNodeId_annotation{"annotation"};
inline constexpr std::string_view kNodeId_complexType{"complexType"};
inline constexpr std::string_view kNodeId_sequence{"sequence"};
inline constexpr std::string_view kNodeId_group{"group"};
inline constexpr std::string_view kNodeId_choice{"choice"};
inline constexpr std::string_view kNodeId_complexContent{"complexContent"};
inline constexpr std::string_view kNodeId_simpleType{"simpleType"};
inline constexpr std::string_view kNodeId_element{"element"};
inline constexpr std::string_view kNodeId_documentation{"documentation"};
inline constexpr std::string_view kNodeId_restriction{"restriction"};

// todo: implement
// not all of them are interesting for our use case but listed for completeness
inline constexpr std::string_view kNodeId_all{"all"};
inline constexpr std::string_view kNodeId_any{"any"};
inline constexpr std::string_view kNodeId_anyAttribute{"anyAttribute"};
inline constexpr std::string_view kNodeId_appinfo{"appinfo"};
inline constexpr std::string_view kNodeId_attributeGroup{"attributeGroup"};
inline constexpr std::string_view kNodeId_enumeration{"enumeration"};
inline constexpr std::string_view kNodeId_extension{"extension"};
inline constexpr std::string_view kNodeId_field{"field"};
inline constexpr std::string_view kNodeId_fractionDigits{"fractionDigits"};
inline constexpr std::string_view kNodeId_key{"key"};
inline constexpr std::string_view kNodeId_keyref{"keyref"};
inline constexpr std::string_view kNodeId_length{"length"};
inline constexpr std::string_view kNodeId_list{"list"};
inline constexpr std::string_view kNodeId_maxExclusive{"maxExclusive"};
inline constexpr std::string_view kNodeId_maxInclusive{"maxInclusive"};
inline constexpr std::string_view kNodeId_maxLength{"maxLength"};
inline constexpr std::string_view kNodeId_minLength{"minLength"};
inline constexpr std::string_view kNodeId_minExclusive{"minExclusive"};
inline constexpr std::string_view kNodeId_minInclusive{"minInclusive"};
inline constexpr std::string_view kNodeId_notation{"notation"};
inline constexpr std::string_view kNodeId_pattern{"pattern"};
inline constexpr std::string_view kNodeId_redefine{"redefine"};
inline constexpr std::string_view kNodeId_selector{"selector"};
inline constexpr std::string_view kNodeId_simpleContent{"simpleContent"};
inline constexpr std::string_view kNodeId_totalDigits{"totalDigits"};
inline constexpr std::string_view kNodeId_union{"union"};
inline constexpr std::string_view kNodeId_unique{"unique"};
inline constexpr std::string_view kNodeId_whitespace{"whitespace"};
// end-todo

// built-in types
inline constexpr std::string_view kBuildinTypeId_base64Binary{"base64Binary"};
inline constexpr std::string_view kBuildinTypeId_string{"string"};
inline constexpr std::string_view kBuildinTypeId_boolean{"boolean"};
inline constexpr std::string_view kBuildinTypeId_byte{"byte"};

inline constexpr std::string_view kBuildinTypeId_date{"date"};
inline constexpr std::string_view kBuildinTypeId_dateTime{"dateTime"};
inline constexpr std::string_view kBuildinTypeId_duration{"duration"};

inline constexpr std::string_view kBuildinTypeId_decimal{"decimal"};
inline constexpr std::string_view kBuildinTypeId_double{"double"};
inline constexpr std::string_view kBuildinTypeId_float{"float"};
inline constexpr std::string_view kBuildinTypeId_int{"int"};
inline constexpr std::string_view kBuildinTypeId_integer{"integer"};
inline constexpr std::string_view kBuildinTypeId_negativeInteger{"negativeInteger"};
inline constexpr std::string_view kBuildinTypeId_nonNegativeInteger{"nonNegativeInteger"};
inline constexpr std::string_view kBuildinTypeId_nonPositiveInteger{"nonPositiveInteger"};
inline constexpr std::string_view kBuildinTypeId_normalizedString{"normalizedString"};
inline constexpr std::string_view kBuildinTypeId_positiveInteger{"positiveInteger"};
inline constexpr std::string_view kBuildinTypeId_long{"long"};
inline constexpr std::string_view kBuildinTypeId_short{"short"};
inline constexpr std::string_view kBuildinTypeId_unsignedByte{"unsignedByte"};
inline constexpr std::string_view kBuildinTypeId_unsignedInt{"unsignedInt"};
inline constexpr std::string_view kBuildinTypeId_unsignedLong{"unsignedLong"};
inline constexpr std::string_view kBuildinTypeId_unsignedShort{"unsignedShort"};
// end built-in types

void process_file(State &state, const fs::path &file_path);
void process_url(State &state, const std::string &file_path);
void process_schema(State &state, const pugi::xml_node &node);
void process_include(State &state, const pugi::xml_node &node);
void process_annotation(State &state, const pugi::xml_node &node);
void process_complexType(State &state, const pugi::xml_node &node);
void process_sequence(State &state, const pugi::xml_node &node);
void process_group(State &state, const pugi::xml_node &node);
void process_choice(State &state, const pugi::xml_node &node);
void process_complexContent(State &state, const pugi::xml_node &node);
void process_simpleType(State &state, const pugi::xml_node &node);
void process_element(State &state, const pugi::xml_node &node);
void process_documentation(State &state, const pugi::xml_node &node);
void process_restriction(State &state, const pugi::xml_node &node);

constexpr bool is_node_type(const std::string_view expected, const std::string_view str)
{
    if (str.size() >= 3 && str.starts_with("xs:"))
    {
        return str.substr(3) == expected;
    }
    return str == expected;
}

constexpr bool is_node_type(const std::vector<std::string_view> expected, const std::string_view str)
{
    for (const auto &e : expected)
        if (is_node_type(e, str))
            return true;
    return false;
}

meta::ElementType type_str_to_element_type(const std::string_view type_str);
} // namespace cppxsd::parser
