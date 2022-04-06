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
    explicit Parser(const fs::path &working_dir);
    ~Parser();
    Parser(const Parser &other) = delete;
    Parser(Parser &&other) noexcept = delete;
    Parser &operator=(const Parser &other) = delete;
    Parser &operator=(Parser &&other) noexcept = delete;

    void parse(const std::string &xsd_file);
    void parseFileContent(const std::string_view xml_content, const std::string_view uri);

  private:
    SchemaPtr tryParseUri(const std::string_view uri);
    void parseFile(const fs::path &xsd_file);
    void parseUrl(const std::string_view file_path);
    void parseDocument(const pugi::xml_document &doc, const std::string_view uri);
    void parseSchema(const pugi::xml_node &node, const std::string_view uri);

    meta::xsd_include parseInclude(const pugi::xml_node &node);
    meta::xsd_import parseImport(const pugi::xml_node &node);
    meta::redefine parseRedefine(const pugi::xml_node &node);
    meta::annotation parseAnnotation(const pugi::xml_node &node) const;
    meta::element parseElement(const pugi::xml_node &node);
    meta::simpleType parseSimpleType(const pugi::xml_node &node, const bool invoked_from_schema) const;
    meta::restriction parseRestriction(const pugi::xml_node &node) const;
    meta::list parseList(const pugi::xml_node& node) const;
    meta::xsd_union parseUnion(const pugi::xml_node& node) const;

    meta::OptionalAnnotation getAnnotation(const pugi::xml_node &node) const;
    meta::OptionalId getId(const pugi::xml_node &node) const;
    SchemaPtr getSchemaFromUri(const std::string_view path) const;

  public:
    State state;

  private:
    const fs::path working_dir_;
    fs::path current_file_dir_;
};

} // namespace cppxsd::parser
