#include <array>
#include <functional>
#include <iostream>
#include "exceptions.hpp"
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_schema(State &state, const pugi::xml_node &node)
{
    using CbPtr = void (*)(State &, const pugi::xml_node &);
    using CbPair = std::pair<std::string_view, CbPtr>;
    constexpr std::array<CbPair, 7> kCallbacks{CbPair{kNodeId_include, process_include},
                                               CbPair{kNodeId_import, process_include},
                                               CbPair{kNodeId_complexType, process_complexType},
                                               CbPair{kNodeId_group, process_group},
                                               CbPair{kNodeId_element, process_element},
                                               CbPair{kNodeId_simpleType, process_simpleType},
                                               CbPair{kNodeId_annotation, process_annotation}};

    for (const auto &n : node)
    {
        const auto node_name = n.name();
        const auto it = std::find_if(kCallbacks.begin(), kCallbacks.end(), [&node_name](const auto &cb) {
            return is_node_type(cb.first, node_name);
        });

        if (it != kCallbacks.end())
            it->second(state, n);
        else
            throw ParseException{kNodeId_schema,
                                 {kNodeId_include,
                                  kNodeId_import,
                                  kNodeId_complexType,
                                  kNodeId_group,
                                  kNodeId_element,
                                  kNodeId_simpleType,
                                  kNodeId_annotation},
                                 n};
    }
}
} // namespace cppxsd::parser
