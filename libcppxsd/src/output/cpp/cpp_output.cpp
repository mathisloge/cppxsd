#include "cpp_output.hpp"

namespace cppxsd::out
{
void CppOutput::operator()(const meta::Element &t)
{
    for (const auto &e : t.childs)
    {
        std::visit([this](auto &x) { (*this)(x); }, e);
    }
}
void CppOutput::operator()(const meta::ElementType &t)
{
    std::visit([this](auto &e) { (*this)(e); }, t);
}
void CppOutput::operator()(const meta::StringType &t)
{}
void CppOutput::operator()(const meta::BoolType &t)
{}
void CppOutput::operator()(const meta::ByteType &t)
{}
void CppOutput::operator()(const meta::ShortType &t)
{}
void CppOutput::operator()(const meta::IntType &t)
{}
void CppOutput::operator()(const meta::LongType &t)
{}
void CppOutput::operator()(const meta::UnsignedByteType &t)
{}
void CppOutput::operator()(const meta::UnsignedShortType &t)
{}
void CppOutput::operator()(const meta::UnsignedIntType &t)
{}
void CppOutput::operator()(const meta::UnsignedLongType &t)
{}
void CppOutput::operator()(const meta::FloatType &t)
{}
void CppOutput::operator()(const meta::DoubleType &t)
{}
void CppOutput::operator()(const meta::ElementRef &t)
{}
void CppOutput::operator()(const meta::ListElement &t)
{}
} // namespace cppxsd::out
