#include "nodes.hpp"
namespace cppxsd::parser
{
void process_schema(State &state, const pugi::xml_node &node)
{
    for (const auto &n : node)
    {
        process_node(kNodeId_schema,
                     {kNodeId_include,
                      kNodeId_import,
                      kNodeId_complexType,
                      kNodeId_group,
                      kNodeId_element,
                      kNodeId_simpleType,
                      kNodeId_annotation},
                     state,
                     n);
    }
}
} // namespace cppxsd::parser
