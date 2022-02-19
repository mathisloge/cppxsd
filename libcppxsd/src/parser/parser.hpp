#pragma once
#include "defs.hpp"
#include "state.hpp"

namespace cppxsd::parser
{
class Parser final
{
  public:
    Parser();
    ~Parser();
    void addFile(const fs::path &xsd_file);

  public:
    State state;
};

} // namespace cppxsd::parser
