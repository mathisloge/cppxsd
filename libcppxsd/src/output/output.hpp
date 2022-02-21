#pragma once
#include "../meta/ast.hpp"
namespace cppxsd::out
{

class OutputVisitor
{
  public:
    virtual ~OutputVisitor() = default;

    virtual void operator()(const meta::CustomType &t) = 0;
    virtual void operator()(const meta::StringType &t) = 0;
    virtual void operator()(const meta::ByteArrayType &t) = 0;
    virtual void operator()(const meta::BoolType &t) = 0;
    virtual void operator()(const meta::ByteType &t) = 0;
    virtual void operator()(const meta::ShortType &t) = 0;
    virtual void operator()(const meta::IntType &t) = 0;
    virtual void operator()(const meta::LongType &t) = 0;
    virtual void operator()(const meta::UnsignedByteType &t) = 0;
    virtual void operator()(const meta::UnsignedShortType &t) = 0;
    virtual void operator()(const meta::UnsignedIntType &t) = 0;
    virtual void operator()(const meta::UnsignedLongType &t) = 0;
    virtual void operator()(const meta::FloatType &t) = 0;
    virtual void operator()(const meta::DoubleType &t) = 0;
    virtual void operator()(const meta::TypeRef &t) = 0;
    virtual void operator()(const meta::ListType &t) = 0;
    virtual void operator()(const meta::ListElement &t) = 0;
    virtual void operator()(const meta::TypeDefinition &t) = 0;
    virtual void operator()(const meta::TimeDurationType &t) = 0;
    virtual void operator()(const meta::UnsupportedBuildinType &t) = 0;
};

class Output : public OutputVisitor
{
  public:
    virtual ~Output() = default;
};

} // namespace cppxsd::out
