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
    using SchemaPtr = std::shared_ptr<meta::schema>;

  public:
    Parser(const fs::path &working_dir);
    ~Parser();
    void parse(const std::string &xsd_file);

  private:
    SchemaPtr tryParseUri(const std::string &uri);
    void parseFile(const fs::path &xsd_file);
    void parseUrl(const std::string &file_path);
    void parseDocument(const pugi::xml_document &doc, const std::string &uri);
    void parseSchema(const pugi::xml_node &node, const std::string &uri);
    void parseElement(const pugi::xml_node &node, const CbMap &callbacks);

    meta::xsd_include parseInclude(const pugi::xml_node &node);
    meta::xsd_import parseImport(const pugi::xml_node &node);
    meta::redefine parseRedefine(const pugi::xml_node &node);
    meta::annotation parseAnnotation(const pugi::xml_node &node);

    meta::OptionalId getId(const pugi::xml_node &node) const;
    SchemaPtr getSchemaFromUri(const std::string_view path) const;

  public:
    State state;

  private:
    const fs::path &working_dir_;
    fs::path current_file_dir_;
};

} // namespace cppxsd::parser
