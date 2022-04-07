#include "exceptions.hpp"
#include <fmt/compile.h>
#include <fmt/format.h>

ParseException::ParseException(const std::string_view current_processing_node_type,
                               const std::set<std::string_view> &node_types,
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

ValidationException::ValidationException(const std::string_view curr_node,
                                         const pugi::xml_node &node,
                                         const std::string_view err_str)
    : msg{fmt::format(FMT_COMPILE("[VALIDATION@{}] {}: {}"), node.path(), curr_node, err_str)}
{}

const char *ValidationException::what() const noexcept
{
    return msg.c_str();
}
