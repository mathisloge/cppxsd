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

    meta::CustomType el{type_name, meta::TypeRef{}, is_abstract};

    const auto anno_node = node.find_child([](const auto &n) { return is_node_type(kNodeId_annotation, n.name()); });
    const auto content_node = node.find_child([](const auto &n) { return is_node_type(kValidChilds, n.name()); });

    if (anno_node)
    {
        process_annotation(state, anno_node);
    }

    if (!content_node)
    {
        std::cout << "C====" << std::endl;
        for (const auto &n : node)
        {
            std::cout << "C " << n.name() << std::endl;
        }
        state.current_el = meta::UnsupportedBuildinType{};
        state.current_el_name = std::move(type_name);
        return;
    }
    const auto cname = content_node.name();
    if (is_node_type(kNodeId_simpleContent, cname))
    {
        PRINT_TODO_NODE(kNodeId_simpleContent);
    }
    else if (is_node_type(kNodeId_complexContent, cname))
    {
        PRINT_TODO_NODE(kNodeId_complexContent);
    }
    else if (is_node_type(kNodeId_group, cname))
    {
        PRINT_TODO_NODE(kNodeId_group);
    }
    else if (is_node_type(kNodeId_all, cname))
    {
        PRINT_TODO_NODE(kNodeId_all);
    }
    else if (is_node_type(kNodeId_choice, cname))
        process_choice(state, content_node);
    else if (is_node_type(kNodeId_sequence, cname))
        process_sequence(state, content_node);
    else
        throw ParseException{kNodeId_complexType,
                             {kNodeId_simpleContent,
                              kNodeId_complexContent,
                              kNodeId_group,
                              kNodeId_all,
                              kNodeId_choice,
                              kNodeId_sequence},
                             content_node};

    el.el_type = std::move(state.current_el);
    state.current_el = std::move(el);
    state.current_el_name = std::move(type_name);
}
} // namespace cppxsd::parser
