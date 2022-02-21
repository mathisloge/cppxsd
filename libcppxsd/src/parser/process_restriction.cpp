#include "nodes.hpp"
namespace cppxsd::parser
{
void process_restriction(State &state, const pugi::xml_node &node)
{
    const auto base_attr = require_attr(kNodeId_restriction, "base", node);
    state.current_el = type_str_to_element_type(base_attr.as_string());
}
} // namespace cppxsd::parser
