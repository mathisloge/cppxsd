#pragma once
#include "../meta/types.hpp"
namespace cppxsd::out
{

class OutputVisitor
{
  public:
    virtual ~OutputVisitor() = default;

    virtual void operator()(const meta::Element &t) = 0;
    virtual void operator()(const meta::ElementType &t) = 0;
    
    virtual void operator()(const meta::StringType &t) = 0;
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
    virtual void operator()(const meta::ElementRef &t) = 0;
    virtual void operator()(const meta::ListElement &t) = 0;
};

class Output : public OutputVisitor
{
  public:
    virtual ~Output() = default;
};

} // namespace cppxsd::out
