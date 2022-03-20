#include "parser.hpp"
#include <array>
#include <iostream>
#include <map>
#include <curl/easy.h>
#include <pugixml.hpp>
#include "helpers.hpp"
#include "keys.hpp"
namespace cppxsd::parser
{
Parser::Parser()
{}
Parser::~Parser()
{}
void Parser::addFile(const fs::path &xsd_file)
{
    try
    {
        parseFile(xsd_file);
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
    std::cout << "processing " << xsd_file << std::endl;
    state.current_file = xsd_file;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_path_str.c_str());
    if (!result)
        throw std::runtime_error("failed to parse");

    parseDocument(doc);

    state.already_parsed.emplace(file_path_str);
}

void Parser::parseDocument(const pugi::xml_document &doc)
{
    for (const auto &node : doc)
    {
        const auto child_type = node_name_to_type(node.name());
        if (child_type == NodeType::schema)
            parseSchema(node);
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
void Parser::parseUrl(const std::string &file_path)
{
    if (state.already_parsed.contains(file_path))
    {
        return;
    }

    std::cout << "request http: " << file_path << std::endl;
    std::string data;
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, file_path.c_str());
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

        parseDocument(doc);

        state.already_parsed.emplace(file_path);
    }

    curl_easy_cleanup(curl);
}

constexpr bool contains_type(const std::vector<NodeType> &types, const NodeType curr_type)
{
    return std::any_of(std::begin(types), std::end(types), [curr_type](const auto type) { return type == curr_type; });
}
void Parser::parseSchema(const pugi::xml_node &node)
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

    current = meta::schema{};

    bool finished_import = false;
    for (const auto &child : node)
    {
        const auto child_type = node_name_to_type(child.name());
        if (!finished_import &&
            contains_type({NodeType::include, NodeType::import, NodeType::redefine, NodeType::annotation}, child_type))
        {
            current.imports.emplace_back(parse_import(child_type, child));
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
            {}
            else if (child_type == NodeType::annotation)
            {
                current.annotations.emplace_back(parseAnnotation(child));
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

    state.schemas.emplace_back(std::move(current));
}

meta::xsd_include Parser::parseInclude(const pugi::xml_node &node)
{
    return meta::xsd_include{};
}
meta::xsd_import Parser::parseImport(const pugi::xml_node &node)
{
    return meta::xsd_import{};
}
meta::redefine Parser::parseRedefine(const pugi::xml_node &node)
{
    return meta::redefine{};
}
meta::annotation Parser::parseAnnotation(const pugi::xml_node &node)
{
    return meta::annotation{};
}
} // namespace cppxsd::parser
