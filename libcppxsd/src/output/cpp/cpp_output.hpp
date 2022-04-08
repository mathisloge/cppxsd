#pragma once
#include "../output.hpp"
#include "struct_builder.hpp"
namespace cppxsd::out
{

class CppOutput : public Output
{
  public:
    void operator()(const meta::schema &t) override;

  private:

  private:
    StructBuilder builder;
};
} // namespace cppxsd::out
