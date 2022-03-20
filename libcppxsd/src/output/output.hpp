#pragma once
#include "../meta/ast.hpp"
namespace cppxsd::out
{

class OutputVisitor
{
  public:
    virtual ~OutputVisitor() = default;

    virtual void operator()(const meta::schema &t) = 0;
};

class Output : public OutputVisitor
{
  public:
    virtual ~Output() = default;
};

} // namespace cppxsd::out
