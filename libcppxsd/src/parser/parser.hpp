#pragma once
#include <map>
#include "defs.hpp"
#include "state.hpp"
namespace cppxsd::parser
{
class Parser final
{
  private:
    using CbPtr = std::function<void(const pugi::xml_node &)>;
    using CbMap = std::map<std::string_view, CbPtr>;

  public:
    Parser();
    ~Parser();
    void addFile(const fs::path &xsd_file);

  private:
    void parseFile(const fs::path &xsd_file);
    void parseUrl(const std::string &file_path);
    void parseDocument(const pugi::xml_document &doc);
    void parseSchema(const pugi::xml_node &node);
    void parseElement(const pugi::xml_node &node, const CbMap &callbacks);

    meta::xsd_include parseInclude(const pugi::xml_node &node);
    meta::xsd_import parseImport(const pugi::xml_node &node);
    meta::redefine parseRedefine(const pugi::xml_node &node);
    meta::annotation parseAnnotation(const pugi::xml_node &node);

  public:
    State state;
    meta::schema current;
};

} // namespace cppxsd::parser
