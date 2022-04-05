#include "parser.hpp"
#include <array>
#include <iostream>
#include <map>
#include <curl/easy.h>
#include <fmt/format.h>
#include <pugixml.hpp>
#include "helpers.hpp"
#include "keys.hpp"
namespace cppxsd::parser
{
static bool is_url(const std::string_view uri)
{
    return uri.starts_with("http://") || uri.starts_with("https://");
}
Parser::Parser(const fs::path &working_dir)
    : working_dir_{working_dir}
{}
Parser::~Parser()
{}
void Parser::parse(const std::string &xsd_file)
{
    try
    {
        tryParseUri(xsd_file);
    }
    catch (const std::exception &ex)
    {
        std::cout << "FAILED: " << ex.what() << std::endl;
    }
}

void Parser::parseFileContent(const std::string_view xml_content, const std::string_view uri)
{
    pugi::xml_document doc;
    try
    {
        pugi::xml_parse_result result = doc.load_string(xml_content.data());
        if (!result)
            throw std::runtime_error("failed to parse");
        parseDocument(doc, uri);
        state.already_parsed.emplace(uri);
    }
    catch (const std::exception &ex)
    {
        std::cout << "FAILED: " << ex.what() << std::endl;
    }
}

void Parser::parseFile(const fs::path &xsd_file)
{
    const std::string file_path_str{xsd_file.string()};
    if (state.already_parsed.contains(file_path_str))
    {
        // todo check if path is same. might contain ../ etc.
        return;
    }

    state.current_file = xsd_file;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(xsd_file.generic_string().c_str());
    if (!result)
        throw std::runtime_error("failed to parse");

    const auto rel_path = fs::relative(xsd_file, working_dir_);
    current_file_dir_ = xsd_file.parent_path();
    const auto rel_file_path_str = rel_path.string();
    parseDocument(doc, rel_file_path_str);

    state.already_parsed.emplace(file_path_str);
}

void Parser::parseDocument(const pugi::xml_document &doc, const std::string_view uri)
{
    for (const auto &node : doc)
    {
        const auto child_type = node_name_to_type(node.name());
        if (child_type == NodeType::schema)
            parseSchema(node, uri);
        else
            throw ParseException{"document", {kNodeId_schema}, node};
    }
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    const size_t realsize = size * nmemb;
    std::string &mem = *reinterpret_cast<std::string *>(userp);
    std::copy(static_cast<char *>(contents), static_cast<char *>(contents) + realsize, std::back_inserter(mem));
    return realsize;
}
void Parser::parseUrl(const std::string_view file_path)
{
    const std::string file_path_str{file_path};
    if (state.already_parsed.contains(file_path_str))
    {
        return;
    }

    std::cout << "request http: " << file_path << std::endl;
    std::string data;
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, file_path_str.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcppxsd-agent/1.0");
    const auto curl_success = curl_easy_perform(curl);

    /* check for errors */
    if (curl_success != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_success));
    }
    else
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(data.c_str());
        if (!result)
            throw std::runtime_error("failed to parse");

        parseDocument(doc, file_path);

        state.already_parsed.emplace(file_path);
    }

    curl_easy_cleanup(curl);
}

constexpr bool contains_type(const std::vector<NodeType> &types, const NodeType curr_type)
{
    return std::any_of(std::begin(types), std::end(types), [curr_type](const auto type) { return type == curr_type; });
}

void Parser::parseSchema(const pugi::xml_node &node, const std::string_view uri)
{
    const auto parse_import = [this](const NodeType type,
                                     const pugi::xml_node &to_parse) -> meta::schema::ImportContent {
        switch (type)
        {
        case NodeType::include:
            return parseInclude(to_parse);
        case NodeType::import:
            return parseImport(to_parse);
        case NodeType::redefine:
            return parseRedefine(to_parse);
        case NodeType::annotation:
            return parseAnnotation(to_parse);
        default:
            throw ParseException{
                kNodeId_schema, {kNodeId_include, kNodeId_import, kNodeId_redefine, kNodeId_annotation}, to_parse};
        }
    };

    const auto parse_content = [this](const NodeType type, const pugi::xml_node &to_parse) -> meta::schema::Content {
        switch (type)
        {
        case NodeType::simpleType:
            return parseSimpleType(to_parse, true);
        case NodeType::element:
            return parseElement(to_parse);

        default:
            throw ParseException{kNodeId_schema,
                                 {kNodeId_simpleType,
                                  kNodeId_complexType,
                                  kNodeId_group,
                                  kNodeId_attributeGroup,
                                  kNodeId_element,
                                  kNodeId_attribute,
                                  kNodeId_notation},
                                 to_parse};
        }
    };

    auto schema = std::make_shared<meta::schema>();
    schema->uri = uri;

    for (const auto &attr : node.attributes())
    {
        constexpr std::string_view kXmlns = "xmlns";
        const std::string_view attr_name{attr.name()};
        if (attr_name.starts_with("xmlns"))
        {
            const std::string namespace_uri = attr.as_string();
            std::optional<std::string> namespace_prefix;
            if (attr_name.size() > (kXmlns.size() + 1))
                namespace_prefix = attr_name.substr(kXmlns.size() + 1);
            schema->namespaces.emplace_back(meta::xmlns_namespace{namespace_prefix, namespace_uri});
        }
    }

    bool finished_import = false;
    for (const auto &child : node)
    {
        const auto child_type = node_name_to_type(child.name());
        if (!finished_import &&
            contains_type({NodeType::include, NodeType::import, NodeType::redefine, NodeType::annotation}, child_type))
        {
            schema->imports.emplace_back(parse_import(child_type, child));
        }
        else
        {
            finished_import = true;
            if (contains_type({NodeType::simpleType,
                               NodeType::complexType,
                               NodeType::group,
                               NodeType::attributeGroup,
                               NodeType::element,
                               NodeType::attribute,
                               NodeType::notation},
                              child_type))
            {
                schema->contents.emplace_back(parse_content(child_type, child));
            }
            else if (child_type == NodeType::annotation)
            {
                schema->annotations.emplace_back(parseAnnotation(child));
            }
            else
            {
                throw ParseException{kNodeId_schema,
                                     {kNodeId_simpleType,
                                      kNodeId_complexType,
                                      kNodeId_group,
                                      kNodeId_attributeGroup,
                                      kNodeId_element,
                                      kNodeId_attribute,
                                      kNodeId_notation,
                                      kNodeId_annotation},
                                     node};
            }
        }
    }

    state.schemas.emplace_back(std::move(schema));
}

meta::xsd_include Parser::parseInclude(const pugi::xml_node &node)
{
    const std::string attr{node.attribute("schemaLocation").as_string()};
    auto schema_include = tryParseUri(attr);
    if (!schema_include)
        throw std::runtime_error(fmt::format("schema with include uri \"{}\" not found", attr));
    return meta::xsd_include{.id = getId(node), .schema = schema_include};
}

meta::xsd_import Parser::parseImport(const pugi::xml_node &node)
{
    const auto namespaceAttr = node.attribute("namespace");
    const auto schemaLocAttr = node.attribute("schemaLocation");

    SchemaPtr schema_import{nullptr};
    if (schemaLocAttr)
    {
        const std::string import_schema_uri = schemaLocAttr.as_string();
        schema_import = tryParseUri(import_schema_uri);
        if (!schema_import)
            throw std::runtime_error(fmt::format("schema with import uri \"{}\" not found", import_schema_uri));
    }

    return meta::xsd_import{.id = getId(node),
                            .namespace_uri = namespaceAttr ? namespaceAttr.as_string() : "",
                            .schema_location = schema_import};
}

meta::redefine Parser::parseRedefine(const pugi::xml_node &node)
{
    return meta::redefine{};
}

meta::annotation Parser::parseAnnotation(const pugi::xml_node &node)
{
    return meta::annotation{};
}

meta::element Parser::parseElement(const pugi::xml_node &node)
{
    return meta::element{};
}
meta::simpleType Parser::parseSimpleType(const pugi::xml_node &node, const bool invoked_from_schema)
{
    const auto parse_content = [this](const NodeType type,
                                      const pugi::xml_node &to_parse) -> meta::simpleType::Content {
        switch (type)
        {
        case NodeType::restriction:
            return meta::restriction{};
        case NodeType::list:
            return meta::list{};
        case NodeType::xsd_union:
            return meta::xsd_union{};

        default:
            throw ParseException{kNodeId_simpleType, {kNodeId_restriction, kNodeId_list, kNodeId_union}, to_parse};
        }
    };

    meta::simpleType simple_type;
    bool found_content = false;

    const auto name = node.attribute("name");
    if (!invoked_from_schema && name)
        throw std::runtime_error("when not invoked from schema, name isn't allowed");
    else if (invoked_from_schema && !name)
        throw std::runtime_error("name attribute is required");
    else
        simple_type.name = name.as_string();

    for (const auto &child : node)
    {
        const auto child_type = node_name_to_type(child.name());

        if (child_type == NodeType::annotation)
        {
            if (simple_type.annotation.has_value())
                throw std::runtime_error("annotation already exists");

            simple_type.annotation = parseAnnotation(child);
        }
        else if (contains_type({NodeType::restriction, NodeType::list, NodeType::xsd_union}, child_type))
        {
            if (found_content)
                throw std::runtime_error("content already exists");
            found_content = true;
            simple_type.content = parse_content(child_type, child);
        }
        else
        {
            throw ParseException{
                kNodeId_simpleType, {kNodeId_annotation, kNodeId_restriction, kNodeId_list, kNodeId_union}, child};
        }
    }

    return simple_type;
}

meta::OptionalId Parser::getId(const pugi::xml_node &node) const
{
    const auto attr = node.attribute("id");
    if (!attr)
        return meta::OptionalId{};
    return meta::OptionalId(meta::datatypes::Id{attr.as_string()});
}

Parser::SchemaPtr Parser::tryParseUri(const std::string_view uri)
{
    auto import_schema = getSchemaFromUri(uri);
    if (import_schema)
        return import_schema;
    if (is_url(uri))
    {
        parseUrl(uri);
    }
    else
    {
        const fs::path include_path{uri};
        if (include_path.is_relative())
        {
            const auto fpath = working_dir_ / current_file_dir_ / include_path;
            parseFile(fpath);
            const auto fpath_str = fpath.string();
            return getSchemaFromUri(fpath_str);
        }
        else
            parseFile(include_path);
    }

    return getSchemaFromUri(uri);
}

Parser::SchemaPtr Parser::getSchemaFromUri(const std::string_view uri) const
{
    const bool uri_is_url = is_url(uri);
    const fs::path curr_p{uri_is_url ? "" : current_file_dir_ / fs::path{uri}};
    const auto it =
        std::find_if(state.schemas.begin(), state.schemas.end(), [this, uri, uri_is_url, &curr_p](const auto &s) {
            const bool it_url = is_url(s->uri);
            if (it_url != uri_is_url) // early out if we want to compare http uri with file uri
                return false;
            if (uri_is_url)
                return s->uri == uri;

            const fs::path s_p{working_dir_ / fs::path{s->uri}};
            return fs::equivalent(s_p, curr_p);
        });
    return (it != state.schemas.end()) ? *it : nullptr;
}
} // namespace cppxsd::parser
