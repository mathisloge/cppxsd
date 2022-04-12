#pragma once
#include "../output.hpp"
namespace cppxsd::out
{

class CppOutput : public Output
{
  public:
    void operator()(const meta::schema &t) override;

};
} // namespace cppxsd::out
