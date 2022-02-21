#include "nodes.hpp"
namespace cppxsd::parser
{
void process_complexType(State &state, const pugi::xml_node &node)
{
    static const std::vector<std::string_view> kValidChilds{
        kNodeId_simpleContent, kNodeId_complexContent, kNodeId_group, kNodeId_all, kNodeId_choice, kNodeId_sequence};
    const auto type_name_attr = node.attribute("name");
    const std::string type_name{type_name_attr ? type_name_attr.as_string() : ""};
    const auto abstract_attr = node.attribute("abstract");
    const bool is_abstract = abstract_attr ? abstract_attr.as_bool() : false;

    meta::CustomType el{type_name, {}, is_abstract};

    const auto anno_node = node_find_child_of(node, {kNodeId_annotation});
    const auto content_node = node_find_child_of(node, kValidChilds);

    if (anno_node)
    {
        process_annotation(state, anno_node);
    }

    if (!content_node)
    {
        state.current_el = meta::UnsupportedBuildinType{};
        state.current_el_name = std::move(type_name);
        return;
    }
    process_node(
        kNodeId_complexType,
        {kNodeId_simpleContent, kNodeId_complexContent, kNodeId_group, kNodeId_all, kNodeId_choice, kNodeId_sequence},
        state,
        content_node);
    el.el_type = std::move(state.current_el);
    state.current_el = std::move(el);
    state.current_el_name = std::move(type_name);
}
} // namespace cppxsd::parser
