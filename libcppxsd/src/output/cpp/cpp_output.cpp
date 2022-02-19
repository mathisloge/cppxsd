#include "cpp_output.hpp"
#include <iostream>
#define PRINT_T(type) std::cout << #type << std::endl;
namespace cppxsd::out
{
void CppOutput::operator()(const meta::CustomType &t)
{
    PRINT_T(CustomType);
    std::cout << "Name: " << t.name << " ";
    boost::apply_visitor((*this), t.el_type);
}

void CppOutput::operator()(const meta::ByteArrayType &t)
{
    PRINT_T(ByteArrayType);
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
void CppOutput::operator()(const meta::TypeRef &t)
{
    PRINT_T(TypeRef);
}
void CppOutput::operator()(const meta::ListType &t)
{
    PRINT_T(ListType);
    for (const auto &e : t)
        (*this)(e.get());
}
void CppOutput::operator()(const meta::ListElement &t)
{
    PRINT_T(ListElement);
    boost::apply_visitor(*this, t.element);
}
void CppOutput::operator()(const meta::TypeDefinition &t)
{
    PRINT_T(TypeDefinition);
    std::cout << t.name << " ";
    boost::apply_visitor(*this, t.element);
}

void CppOutput::operator()(const meta::TimeDurationType &t)
{
    PRINT_T(TimeDurationType);
}

void CppOutput::operator()(const meta::UnsupportedBuildinType &t)
{
    PRINT_T(UnsupportedBuildinType);
}
} // namespace cppxsd::out
