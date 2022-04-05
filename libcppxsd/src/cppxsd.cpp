#include "cppxsd/cppxsd.hpp"
#include <iostream>
#include "parser/parser.hpp"

#include "output/cpp/cpp_output.hpp"
namespace cppxsd
{
SchemaContainer parse(const fs::path &file)
{
    parser::Parser p{file};

    for (auto const &dir_entry : fs::recursive_directory_iterator(file))
    {
        std::cout << dir_entry << '\n';
        if (dir_entry.is_regular_file())
            p.parse(dir_entry.path().string());
    }

    out::CppOutput o{};

    for (const auto &e : p.state.schemas)
        o(*e);
    return p.state.schemas;
}

SchemaContainer LIBCPPXSD_EXPORT parse(const std::string_view xml_content, const std::string_view uri)
{
    parser::Parser p{};

    p.parseFileContent(xml_content, uri);

    return p.state.schemas;
}
} // namespace cppxsd
