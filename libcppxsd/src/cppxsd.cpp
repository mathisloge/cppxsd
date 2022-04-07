#include "cppxsd/cppxsd.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include "parser/parser.hpp"

#include "output/cpp/cpp_output.hpp"
namespace cppxsd
{
SchemaContainer parse(const fs::path &file)
{
    if (fs::is_directory(file))
    {
        parser::Parser p{file};

        for (auto const &dir_entry : fs::recursive_directory_iterator(file))
        {
            spdlog::info("processing: {}", dir_entry.path().string());
            if (dir_entry.is_regular_file())
            {
                spdlog::stopwatch sw;
                p.parse(dir_entry.path().string());
                spdlog::info("Needed: {} seconds", sw);
            }
        }

        out::CppOutput o{};

        for (const auto &e : p.state.schemas)
            o(*e);
        return p.state.schemas;
    }
    else
    {
        const auto dir = file.is_relative() ? fs::current_path() : file.parent_path();
        parser::Parser p{dir};
        p.parse(file.string());
        return p.state.schemas;
    }
}

SchemaContainer LIBCPPXSD_EXPORT parse(const std::string_view xml_content, const std::string_view uri)
{
    parser::Parser p{fs::current_path()};

    p.parseFileContent(xml_content, uri);

    return p.state.schemas;
}
} // namespace cppxsd
