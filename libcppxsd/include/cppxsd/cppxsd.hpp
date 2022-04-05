#pragma once
#include <filesystem>
#include <string_view>
#include "ast.hpp"
#include "libcppxsd_export.h"
namespace cppxsd
{
using SchemaContainer = std::vector<std::shared_ptr<meta::schema>>;
SchemaContainer LIBCPPXSD_EXPORT parse(const std::filesystem::path &file);
SchemaContainer LIBCPPXSD_EXPORT parse(const std::string_view xml_content, const std::string_view uri);
} // namespace cppxsd
