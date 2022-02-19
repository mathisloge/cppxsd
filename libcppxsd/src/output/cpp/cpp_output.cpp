#include "cpp_output.hpp"
#include <iostream>
#define PRINT_T(type) std::cout << #type << std::endl;
namespace cppxsd::out
{
void CppOutput::operator()(const meta::ElementType &t)
{
    boost::apply_visitor((*this), t);
}
void CppOutput::operator()(const meta::Element &t)
{
    PRINT_T(Element);
    for (const auto &e : t.childs)
    {
        std::cout << e.first << " => ";
        boost::apply_visitor((*this), e.second);
    }
}
void CppOutput::operator()(const meta::StringType &t)
{
    PRINT_T(StringType);
}
void CppOutput::operator()(const meta::BoolType &t)
{
    PRINT_T(BoolType);
}
void CppOutput::operator()(const meta::ByteType &t)
{
    PRINT_T(ByteType);
}
void CppOutput::operator()(const meta::ShortType &t)
{
    PRINT_T(ShortType);
}
void CppOutput::operator()(const meta::IntType &t)
{
    PRINT_T(IntType);
}
void CppOutput::operator()(const meta::LongType &t)
{
    PRINT_T(LongType);
}
void CppOutput::operator()(const meta::UnsignedByteType &t)
{
    PRINT_T(UnsignedByteType);
}
void CppOutput::operator()(const meta::UnsignedShortType &t)
{
    PRINT_T(UnsignedShortType);
}
void CppOutput::operator()(const meta::UnsignedIntType &t)
{
    PRINT_T(UnsignedIntType);
}
void CppOutput::operator()(const meta::UnsignedLongType &t)
{
    PRINT_T(UnsignedLongType);
}
void CppOutput::operator()(const meta::FloatType &t)
{
    PRINT_T(FloatType);
}
void CppOutput::operator()(const meta::DoubleType &t)
{
    PRINT_T(DoubleType);
}
void CppOutput::operator()(const meta::ElementRef &t)
{
    PRINT_T(ElementRef);
}
void CppOutput::operator()(const meta::ListElement &t)
{
    PRINT_T(ListElement);
}
void CppOutput::operator()(const meta::UnsupportedBuildinType &t)
{
    PRINT_T(UnsupportedBuildinType);
}
} // namespace cppxsd::out
