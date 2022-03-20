#pragma once
#include <string_view>
#include <vector>
namespace cppxsd::parser
{

constexpr bool is_node_type(const std::string_view expected, const std::string_view str)
{
    if (str.size() >= 3 && str.starts_with("xs:"))
    {
        return str.substr(3) == expected;
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
} // namespace cppxsd::parser
