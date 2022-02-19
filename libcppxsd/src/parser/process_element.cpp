#include "nodes.hpp"
namespace cppxsd::parser
{
void process_element(State &state, const pugi::xml_node &node)
{
    const auto type_name_attr = node.attribute("name");
    if (!type_name_attr)
        throw ParseAttrException{kNodeId_element, "name", node};
    const std::string type_name{type_name_attr.as_string()};

    const auto type_attr = node.attribute("type");
    if (!type_attr)
    {
        auto inline_type_node = node.find_child([](const auto &n) {
            return is_node_type({kNodeId_simpleType, kNodeId_complexType}, n.name());
        });
        if (!inline_type_node)
            throw ParseAttrException(kNodeId_element, "type", node);

        const auto type_node_name = inline_type_node.name();
        if (is_node_type(kNodeId_simpleType, type_node_name))
        {
            process_simpleType(state, inline_type_node);
        }
        else if (is_node_type(kNodeId_complexType, type_node_name))
        {
            process_complexType(state, inline_type_node);
        }
        else
            throw ParseException(kNodeId_element, {kNodeId_simpleType, kNodeId_complexType}, inline_type_node);

        state.current_el = meta::CustomType{type_name, meta::TypeRef{}, false, std::move(state.current_el)};
        state.current_el_name = type_name;
    }
    else
    {
        state.current_el_name = type_name;
        state.current_el = meta::TypeRef{type_name, std::string{type_attr.as_string()}};
    }
}
} // namespace cppxsd::parser
