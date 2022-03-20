#pragma once
#include <set>
#include <string_view>
#include <pugixml.hpp>
#include "../meta/ast.hpp"
#include "defs.hpp"
#include "state.hpp"
#include "keys.hpp"
namespace cppxsd::parser
{
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
void process_extension(State &state, const pugi::xml_node &node);
void process_documentation(State &state, const pugi::xml_node &node);
void process_restriction(State &state, const pugi::xml_node &node);

void process_node(const std::string_view curr_node,
                  const std::set<std::string_view> allowed,
                  State &state,
                  const pugi::xml_node &node);
meta::ElementType type_str_to_element_type(const std::string_view type_str);
pugi::xml_attribute require_attr(const std::string_view curr_node,
                                 const std::string_view attr,
                                 const pugi::xml_node &node);
pugi::xml_node node_find_child_of(const pugi::xml_node &node, const std::vector<std::string_view> find);
} // namespace cppxsd::parser
