#include "cpp_output.hpp"
#include <iostream>
#define PRINT_T(type) std::cout << #type << std::endl;
namespace cppxsd::out
{
void CppOutput::operator()(const meta::schema &t)
{
    PRINT_T(schema);
    //boost::apply_visitor((*this), t.el_type);
}
} // namespace cppxsd::out
