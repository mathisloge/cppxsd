#pragma once
#include "libcppxsd_export.h"
#include <filesystem>
namespace cppxsd
{
    void LIBCPPXSD_EXPORT parse(const std::filesystem::path& file);
} // namespace cppxsd
