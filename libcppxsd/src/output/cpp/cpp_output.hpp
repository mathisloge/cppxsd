#pragma once
#include "../output.hpp"
#include "struct_builder.hpp"
namespace cppxsd::out
{
class CppOutput : public Output
{
  public:
    void operator()(const meta::ElementType &t) override;
    void operator()(const meta::Element &t) override;
    void operator()(const meta::StringType &t) override;
    void operator()(const meta::BoolType &t) override;
    void operator()(const meta::ByteType &t) override;
    void operator()(const meta::ShortType &t) override;
    void operator()(const meta::IntType &t) override;
    void operator()(const meta::LongType &t) override;
    void operator()(const meta::UnsignedByteType &t) override;
    void operator()(const meta::UnsignedShortType &t) override;
    void operator()(const meta::UnsignedIntType &t) override;
    void operator()(const meta::UnsignedLongType &t) override;
    void operator()(const meta::FloatType &t) override;
    void operator()(const meta::DoubleType &t) override;
    void operator()(const meta::ElementRef &t) override;
    void operator()(const meta::ListElement &t) override;
    void operator()(const meta::UnsupportedBuildinType &t) override;

  private:
    StructBuilder builder;
};
} // namespace cppxsd::out
