#pragma once
#include <string_view>
#include <pugixml.hpp>
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

constexpr bool is_node_type(const std::string_view expected, const std::string_view str)
{
    if (str.size() >= 3 && str.starts_with("xs:"))
    {
        return str.substr(3) == expected;
    }
    return str == expected;
}
} // namespace cppxsd::parser
