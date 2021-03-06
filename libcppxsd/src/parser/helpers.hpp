#pragma once
#include <ranges>
#include <string_view>
#include <vector>
#include "keys.hpp"
namespace cppxsd::parser
{

constexpr bool is_node_type(const std::string_view expected, const std::string_view str)
{
    const auto found_ns = str.find_first_of(':');
    if (found_ns != std::string_view::npos)
    {
        const auto type_begin_pos = found_ns + 1;
        // drop ':'
        if (str.size() < type_begin_pos)
            return false;
        return str.substr(type_begin_pos) == expected;
    }
    return str == expected;
}

constexpr std::string_view is_node_type(const std::vector<std::string_view> expected, const std::string_view str)
{
    for (const auto &e : expected)
        if (is_node_type(e, str))
            return e;
    return "";
}

inline constexpr std::string_view kEmptyNamespace = "";
constexpr std::string_view get_namespace_prefix(const std::string_view node_name)
{

    const auto ns_delim = node_name.find_first_of(':');
    if (ns_delim == std::string_view::npos)
        return kEmptyNamespace;
    return node_name.substr(0, ns_delim);
}

constexpr bool is_url(const std::string_view uri)
{
    return uri.starts_with("http://") || uri.starts_with("https://");
}
constexpr bool contains_type(const std::vector<NodeType> &types, const NodeType curr_type)
{
    return std::ranges::any_of(types, [curr_type](const auto type) { return type == curr_type; });
}
constexpr bool isValidQName(std::string_view qname)
{
    return true; //! TODO: add qname validation
}
} // namespace cppxsd::parser
