#include "cppxsd/cppxsd.hpp"
#include <iostream>
#include "parser/parser.hpp"

#include "output/cpp/cpp_output.hpp"
namespace cppxsd
{
void parse(const fs::path &file)
{
    parser::Parser p{};

    for (auto const &dir_entry : fs::recursive_directory_iterator(file))
    {
        std::cout << dir_entry << '\n';
        if (dir_entry.is_regular_file())
            p.addFile(dir_entry.path());
    }

    out::CppOutput o{};

    for (const auto &e : p.state.schemas)
        o(e);
}
} // namespace cppxsd
