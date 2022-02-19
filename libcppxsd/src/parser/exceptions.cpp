#include "exceptions.hpp"
#include <fmt/compile.h>
#include <fmt/format.h>

ParseException::ParseException(const std::string_view current_processing_node_type,
                               const std::vector<std::string_view> &node_types,
                               const pugi::xml_node &node)
    : msg{fmt::format(FMT_COMPILE("{}: expected: {}, got: {}; path: {}"),
                      current_processing_node_type,
                      fmt::join(node_types, ", "),
                      node.name(),
                      node.path())}
{}

const char *ParseException::what() const noexcept
{
    return msg.c_str();
}

ParseAttrException::ParseAttrException(const std::string_view current_processing_node_type,
                                       const std::string_view attr,
                                       const pugi::xml_node &node)
    : msg{fmt::format(
          FMT_COMPILE("{}: attribute failed: {} at node: {}"), current_processing_node_type, attr, node.name())}
{}

const char *ParseAttrException::what() const noexcept
{
    return msg.c_str();
}
