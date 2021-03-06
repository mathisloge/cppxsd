#include "parser.hpp"
#include <array>
#include <map>
#include <ranges>
#include <curl/easy.h>
#include <fmt/format.h>
#include <pugixml.hpp>
#include <spdlog/spdlog.h>
#include "helpers.hpp"
#include "keys.hpp"
namespace cppxsd::parser
{

Parser::Parser(const fs::path &working_dir)
    : working_dir_{working_dir}
    , state{nullptr}
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
        spdlog::error("Failed to parse: {}", ex.what());
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
        spdlog::error("Failed to parse: {}", ex.what());
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

        auto schema_before = state.current_schema; // save the current schema.
        if (child_type == NodeType::schema)
            parseSchema(node, uri);
        else
            throw ParseException{"document", {kNodeId_schema}, node};
        state.current_schema = schema_before; // restore the previous schema or nothing if leaving the tree
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

    spdlog::info("[HTTP] request: {}", file_path);
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
        spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(curl_success));
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
        case NodeType::complexType:
            return parseComplexType(to_parse);
        case NodeType::group:
            return parseGroup(to_parse);
        case NodeType::attributeGroup:
            throw std::runtime_error(fmt::format("{}: NOT IMPLEMENTED", kNodeId_attributeGroup));
        case NodeType::element:
            return parseElement(to_parse);
        case NodeType::attribute:
            throw std::runtime_error(fmt::format("{}: NOT IMPLEMENTED", kNodeId_attribute));
        case NodeType::notation:
            throw std::runtime_error(fmt::format("{}: NOT IMPLEMENTED", kNodeId_notation));

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
    state.current_schema = schema;
    schema->uri = uri;
    
    // attribute: xmlns
    for (const auto &attr : node.attributes())
    {
        constexpr std::string_view kXmlns = "xmlns";

        const std::string_view attr_name{attr.name()};
        if (attr_name.starts_with("xmlns"))
        {
            std::optional<std::string> namespace_prefix;
            if (attr_name.size() > (kXmlns.size() + 1))
                namespace_prefix = attr_name.substr(kXmlns.size() + 1);

            const std::string namespace_uri = attr.as_string();
            schema->namespaces.emplace_back(std::make_shared<meta::xmlns_namespace>(namespace_prefix, namespace_uri));
        }
    }
    // END attribute: xmlns

    // attribute: targetNamespace
    const auto targetNamespaceAttr = node.attribute("targetNamespace");
    if (targetNamespaceAttr)
    {
        const std::string targetNamespaceStr = targetNamespaceAttr.as_string();
        auto it = std::find_if(schema->namespaces.begin(),
                               schema->namespaces.end(),
                               [&targetNamespaceStr](const std::shared_ptr<meta::xmlns_namespace> &ns) {
                                   return ns->uri == targetNamespaceStr;
                               });
        if (it == schema->namespaces.end())
            throw ValidationException{kNodeId_schema, node, "can't find a matching namespace for targetNamespace"};
        schema->targetNamespace = *it;
    }
    // END attribute: targetNamespace

    // handle childs
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
    // end handle childs

    state.schemas.emplace_back(std::move(schema));
}

meta::xsd_include Parser::parseInclude(const pugi::xml_node &node)
{
    const std::string attr{node.attribute("schemaLocation").as_string()};
    auto schema_include = tryParseUri(attr);
    if (!schema_include)
        throw ValidationException{kNodeId_include, node, fmt::format("schema with include uri \"{}\" not found", attr)};
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
            throw ValidationException{
                kNodeId_import, node, fmt::format("schema with import uri \"{}\" not found", import_schema_uri)};
    }

    return meta::xsd_import{.id = getId(node),
                            .namespace_uri = namespaceAttr ? namespaceAttr.as_string() : "",
                            .schema_location = schema_import};
}

meta::redefine Parser::parseRedefine(const pugi::xml_node &node)
{
    return meta::redefine{};
}

meta::annotation Parser::parseAnnotation(const pugi::xml_node &node) const
{
    return meta::annotation{};
}

//! https://www.w3schools.com/xml/el_element.asp
meta::element Parser::parseElement(const pugi::xml_node &node) const
{
    meta::element item;

    item.id = getId(node);
    item.name = node.attribute("name").as_string();
    {
        const auto refAttr = node.attribute("ref");
        if (refAttr)
        {
            item.ref = meta::QName{refAttr.as_string()};
            if (!isValidQName(item.ref->name))
                throw ValidationException{
                    kNodeId_element, node, fmt::format("ref: can't find qname of {}", item.ref->name)};
        }
    }
    {
        const auto typeAttr = node.attribute("type");
        if (typeAttr)
        {
            item.type = meta::QName{typeAttr.as_string()};
            if (!isValidQName(item.type->name))
                throw ValidationException{
                    kNodeId_element, node, fmt::format("type: can't find qname of {}", item.type->name)};
        }
    }
    { //! https://stackoverflow.com/questions/39868769/xsd-element-substitution-group-example
        const auto subsGroupAttr = node.attribute("substitutionGroup");
        if (subsGroupAttr)
        {
            item.substitution_group = meta::QName{subsGroupAttr.as_string()};
            if (!isValidQName(item.substitution_group->name))
                throw ValidationException{
                    kNodeId_element,
                    node,
                    fmt::format("substitution_group: can't find qname of {}", item.substitution_group->name)};
        }
    }

    item.is_abstract = node.attribute("abstract").as_bool(false);
    //! TODO: implement all attributes

    item.annotation = getAnnotation(node);

    const auto contentNode = node.find_child([](const pugi::xml_node &child) {
        const auto t = node_name_to_type(child.name());
        return contains_type({NodeType::simpleType, NodeType::complexType}, t);
    });
    if (contentNode)
    {
        item.content = [this](const pugi::xml_node &child) -> meta::element::Content {
            const auto type = node_name_to_type(child.name());
            switch (type)
            {
            case NodeType::simpleType:
                return parseSimpleType(child, false);
            case NodeType::complexType:
                return parseComplexType(child);
            default:
                throw ParseException{kNodeId_element, {kNodeId_simpleType, kNodeId_complexType}, child};
            }
        }(contentNode);
    }

    return item;
}

//! https://www.w3schools.com/xml/el_complextype.asp
meta::complexType Parser::parseComplexType(const pugi::xml_node &node) const
{
    meta::complexType item{};

    item.id = getId(node);
    item.name = node.attribute("name").as_string();
    item.abstract = node.attribute("abstract").as_bool(false);
    item.mixed = node.attribute("mixed").as_bool(false);
    item.annotation = getAnnotation(node);
    item.attributes = getGeneralAttributes(node);

    if (item.mixed)
    {
        const auto simpleContentNode = node.find_child([](const pugi::xml_node &child) {
            const auto t = node_name_to_type(child.name());
            return contains_type({NodeType::simpleContent}, t);
        });
        if (simpleContentNode)
            throw ValidationException{kNodeId_complexType, node, "simpleContent is not allowed when mixed is set"};
    }

    // is this even correct? => https://www.w3.org/TR/xmlschema-0/#restrictingTypeDerivs
    const auto parseBlockType = [&node](const std::string_view str) {
        if (str.empty())
            return meta::complexType::BlockType::schemaDefault;
        else if (str == "extension")
            return meta::complexType::BlockType::extension;
        else if (str == "restriction")
            return meta::complexType::BlockType::restriction;
        else if (str == "#all")
            return meta::complexType::BlockType::all;
        else
            throw ValidationException{kNodeId_complexType, node, fmt::format("couldn't parse block type: {}", str)};
    };
    item.blockType = parseBlockType(node.attribute("block").as_string());
    item.finalType = parseBlockType(node.attribute("final").as_string());

    const auto contentNode = node.find_child([](const auto child) {
        const auto t = node_name_to_type(child.name());
        return contains_type({NodeType::simpleContent, NodeType::complexContent}, t);
    });
    if (contentNode)
    {
        item.content = [this](const pugi::xml_node &child) -> meta::complexType::Content {
            const auto t = node_name_to_type(child.name());
            switch (t)
            {
            case NodeType::simpleContent:
                return parseSimpleContent(child);
            case NodeType::complexContent:
                return parseComplexContent(child);
            default:
                throw ParseException{kNodeId_complexType, {kNodeId_simpleContent, kNodeId_complexContent}, child};
            }
        }(contentNode);
    }
    else
    {
        const auto subContentNode = node.find_child([](const auto child) {
            const auto t = node_name_to_type(child.name());
            return contains_type({NodeType::group, NodeType::all, NodeType::choice, NodeType::sequence}, t);
        });
        if (subContentNode)
        {
            item.content = [this](const pugi::xml_node &child) -> meta::complexType::SubContent {
                const auto t = node_name_to_type(child.name());
                switch (t)
                {
                case NodeType::group:
                    return parseGroup(child);
                case NodeType::all:
                    return parseAll(child);
                case NodeType::choice:
                    return parseChoice(child);
                case NodeType::sequence:
                    return parseSequence(child);
                default:
                    throw ParseException{
                        kNodeId_complexType, {kNodeId_group, kNodeId_all, kNodeId_choice, kNodeId_sequence}, child};
                }
            }(subContentNode);
        }
        item.attributes = getGeneralAttributes(node);
    }

    return item;
}

//! https://www.w3schools.com/xml/el_simpletype.asp
meta::simpleType Parser::parseSimpleType(const pugi::xml_node &node, const bool invoked_from_schema) const
{
    const auto parse_content = [this](const NodeType type,
                                      const pugi::xml_node &to_parse) -> meta::simpleType::Content {
        switch (type)
        {
        case NodeType::restriction:
            return parseRestriction(to_parse);
        case NodeType::list:
            return parseList(to_parse);
        case NodeType::xsd_union:
            return parseUnion(to_parse);

        default:
            throw ParseException{kNodeId_simpleType, {kNodeId_restriction, kNodeId_list, kNodeId_union}, to_parse};
        }
    };

    meta::simpleType simple_type{};
    bool found_content = false;

    const auto name = node.attribute("name");
    if (!invoked_from_schema && name)
        throw ValidationException{kNodeId_simpleType, node, "when not invoked from schema, name isn't allowed"};
    else if (invoked_from_schema && !name)
        throw ParseAttrException(kNodeId_simpleType, "name", node);
    else
        simple_type.name = name.as_string();

    for (const auto &child : node)
    {
        const auto child_type = node_name_to_type(child.name());

        if (child_type == NodeType::annotation)
        {
            if (simple_type.annotation.has_value())
                throw ValidationException{kNodeId_simpleType, node, "annotation already exists"};

            simple_type.annotation = parseAnnotation(child);
        }
        else if (contains_type({NodeType::restriction, NodeType::list, NodeType::xsd_union}, child_type))
        {
            if (found_content)
                throw ValidationException{kNodeId_simpleType, node, "content already exists"};
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

meta::restriction Parser::parseRestriction(const pugi::xml_node &node) const
{
    meta::restriction restriction{};

    const auto baseAttr = node.attribute("base");
    if (!baseAttr)
        throw ParseAttrException(kNodeId_restriction, "base", node);
    const std::string_view base = baseAttr.as_string();

    if (!isValidQName(base))
        throw ValidationException{kNodeId_restriction, node, fmt::format("can't find qname of {}", base)};
    restriction.base = meta::QName{std::string{base}};

    return restriction;
}

meta::list Parser::parseList(const pugi::xml_node &node) const
{
    const auto itemTypeAttr = node.attribute("itemType");
    const auto nodeSimpleType = node.find_child(
        [](const pugi::xml_node &node) { return node_name_to_type(node.name()) == NodeType::simpleType; });

    if (itemTypeAttr && nodeSimpleType)
        throw ValidationException{
            kNodeId_list, node, "itemType Attribute and simpleType are not allowed at the same time"};
    else if (!itemTypeAttr && !nodeSimpleType)
        throw ValidationException{kNodeId_list, node, "either itemType Attribute or simpleType have to be present"};

    meta::list item{};
    item.annotation = getAnnotation(node);
    if (itemTypeAttr)
    {
        const std::string_view qname = itemTypeAttr.as_string();
        if (!isValidQName(qname))
            throw ValidationException{kNodeId_list, node, fmt::format("Not a valid qname: {}", qname)};
        item.baseType = meta::QName{std::string{qname}};
    }
    else if (nodeSimpleType)
    {
        item.baseType = parseSimpleType(nodeSimpleType, false);
    }

    return item;
}

meta::xsd_union Parser::parseUnion(const pugi::xml_node &node) const
{
    meta::xsd_union item{};
    item.id = getId(node);
    item.annotation = getAnnotation(node);

    const auto memberTypesAttr = node.attribute("memberTypes");
    if (!memberTypesAttr)
        throw ParseAttrException{kNodeId_union, "memberTypes", node};
    const std::string_view memberTypes = memberTypesAttr.value();
    for (const auto qname_view : std::views::split(memberTypes, ' '))
    {
        const std::string_view qname_str{qname_view.begin(), qname_view.end()};
        if (!isValidQName(qname_str))
            throw ValidationException{kNodeId_union, node, fmt::format("not a valid qname: {}", qname_str)};
        item.memberTypes.emplace_back(meta::QName{std::string{qname_str}});
    }
    return item;
}

void Parser::parseCommonContent(meta::common_content &item, const pugi::xml_node &node) const
{
    item.id = getId(node);
    item.annotation = getAnnotation(node);

    const auto nodeRestriction = node.find_child(
        [](const pugi::xml_node &node) { return node_name_to_type(node.name()) == NodeType::restriction; });
    const auto nodeExtension = node.find_child(
        [](const pugi::xml_node &node) { return node_name_to_type(node.name()) == NodeType::extension; });
    if (nodeRestriction)
        item.content = parseRestriction(nodeRestriction);
    else if (nodeExtension)
        item.content = parseExtension(nodeExtension);
    else
        throw ValidationException{kNodeId_simpleContent, node, "missing restriction or extension"};
}

//! https://www.w3schools.com/xml/el_simplecontent.asp
meta::simpleContent Parser::parseSimpleContent(const pugi::xml_node &node) const
{
    meta::simpleContent item{};
    parseCommonContent(item, node);

    return item;
}

//! https://www.w3schools.com/xml/el_complexcontent.asp
meta::complexContent Parser::parseComplexContent(const pugi::xml_node &node) const
{
    meta::complexContent item{};
    parseCommonContent(item, node);

    item.mixed = node.attribute("mixed").as_bool(false);

    return item;
}

//! https://www.w3schools.com/xml/el_extension.asp
meta::extension Parser::parseExtension(const pugi::xml_node &node) const
{
    meta::extension item{};
    item.id = getId(node);
    item.annotation = getAnnotation(node);

    const auto contentNode = node.find_child([](const pugi::xml_node &child) {
        const auto t = node_name_to_type(child.name());
        return contains_type({NodeType::group, NodeType::all, NodeType::choice, NodeType::sequence}, t);
    });
    if (contentNode)
    {
        item.content = [this](const pugi::xml_node &child) -> meta::extension::Content {
            const auto ntype = node_name_to_type(child.name());
            switch (ntype)
            {
            case NodeType::group:
                return parseGroup(child);
            case NodeType::all:
                return parseAll(child);
            case NodeType::choice:
                return parseChoice(child);
            case NodeType::sequence:
                return parseSequence(child);
            default:
                throw ParseException{
                    kNodeId_extension, {kNodeId_group, kNodeId_all, kNodeId_choice, kNodeId_sequence}, child};
            }
        }(contentNode);
    }

    item.attributes = getGeneralAttributes(node);

    return item;
}

//! https://www.w3schools.com/xml/el_choice.asp
meta::choice Parser::parseChoice(const pugi::xml_node &node) const
{
    meta::choice item{};
    parseCommonSequence(item, node);
    return item;
}

//! https://www.w3schools.com/xml/el_sequence.asp
meta::sequence Parser::parseSequence(const pugi::xml_node &node) const
{
    meta::sequence item{};
    parseCommonSequence(item, node);
    return item;
}

void Parser::parseCommonSequence(meta::common_sequence &item, const pugi::xml_node &node) const
{
    item.id = getId(node);
    item.annotation = getAnnotation(node);

    const auto max_occurs_attr = node.attribute("maxOccurs");
    const auto min_occurs_attr = node.attribute("minOccurs");
    const std::string_view max_occ_str = max_occurs_attr.value();
    if (max_occ_str == "unbounded")
        item.max_occurs = meta::common_sequence::kUnbounded;
    else
        item.max_occurs = max_occurs_attr.as_ullong(1);
    item.min_occurs = min_occurs_attr.as_ullong(1);

    const auto parseContent = [this](const pugi::xml_node &child) -> meta::common_sequence::Content {
        const auto ntype = node_name_to_type(child.name());
        switch (ntype)
        {
        case NodeType::element:
            return parseElement(child);
        case NodeType::group:
            return parseGroup(child);
        case NodeType::choice:
            return parseChoice(child);
        case NodeType::sequence:
            return parseSequence(child);
        case NodeType::any:
            return parseAny(child);
        default:
            throw ParseException{
                kNodeId_choice, {kNodeId_element, kNodeId_group, kNodeId_choice, kNodeId_sequence, kNodeId_any}, child};
        }
    };

    for (const pugi::xml_node child : node)
    {
        const auto ntype = node_name_to_type(child.name());
        if (contains_type(
                std::vector<NodeType>{
                    NodeType::element, NodeType::group, NodeType::choice, NodeType::sequence, NodeType::any},
                ntype))
        {

            item.content.emplace_back(parseContent(child));
        }
    }
}

//! https://www.w3schools.com/xml/el_all.asp
meta::all Parser::parseAll(const pugi::xml_node &node) const
{
    meta::all item{};
    item.id = getId(node);
    item.annotation = getAnnotation(node);

    const auto maxOccursAttr = node.attribute("maxOccurs");
    const auto minOccursAttr = node.attribute("minOccurs");

    item.max_occurs = maxOccursAttr.as_ullong(1);
    item.min_occurs = minOccursAttr.as_ullong(1);
    if (item.max_occurs != 1)
        throw ValidationException{kNodeId_all, node, "maxOccurs value must be 1."};

    if (item.min_occurs > 1)
        throw ValidationException{kNodeId_all, node, "minOccurs value must be 0 or 1."};

    for (const auto child : node)
    {
        const auto ctype = node_name_to_type(child.name());
        if (ctype == NodeType::element)
            item.content.emplace_back(parseElement(child));
    }

    return item;
}

//! https://www.w3schools.com/xml/el_group.asp
meta::group Parser::parseGroup(const pugi::xml_node &node) const
{
    meta::group item{};
    item.id = getId(node);
    item.annotation = getAnnotation(node);

    const auto max_occurs_attr = node.attribute("maxOccurs");
    const auto min_occurs_attr = node.attribute("minOccurs");
    const std::string_view max_occ_str = max_occurs_attr.value();
    if (max_occ_str == "unbounded")
        item.max_occurs = meta::common_sequence::kUnbounded;
    else
        item.max_occurs = max_occurs_attr.as_ullong(1);
    item.min_occurs = min_occurs_attr.as_ullong(1);

    const auto refAttr = node.attribute("ref");
    const auto nameAttr = node.attribute("name");
    if (refAttr && nameAttr)
        throw ValidationException{kNodeId_group, node, "name and ref can't be present at the same time"};
    item.ref = meta::QName{refAttr.as_string()};
    item.name = nameAttr.as_string();

    if (refAttr && !isValidQName(item.ref.name))
        throw ValidationException{kNodeId_group, node, "ref must be a valid QName"};

    const auto cnode = node.find_child([](const pugi::xml_node &node) {
        const auto tnode = node_name_to_type(node.name());
        return contains_type(std::vector<NodeType>{NodeType::all, NodeType::choice, NodeType::sequence}, tnode);
    });
    if (cnode)
    {
        item.content = [this](const pugi::xml_node &child) -> meta::group::Content {
            const auto tnode = node_name_to_type(child.name());
            switch (tnode)
            {
            case NodeType::all:
                return parseAll(child);
            case NodeType::choice:
                return parseChoice(child);
            case NodeType::sequence:
                return parseSequence(child);
            default:
                throw ParseException{kNodeId_group, {kNodeId_choice, kNodeId_sequence, kNodeId_all}, child};
            }
        }(cnode);
    }

    return item;
}
meta::any Parser::parseAny(const pugi::xml_node &node) const
{
    throw std::runtime_error("currently not supported");
    return meta::any{};
}

meta::GeneralAttributes Parser::getGeneralAttributes(const pugi::xml_node &node) const
{
    return meta::GeneralAttributes{};
}

meta::OptionalAnnotation Parser::getAnnotation(const pugi::xml_node &node) const
{
    const auto attr_node = node.find_child(
        [](const pugi::xml_node &node) { return node_name_to_type(node.name()) == NodeType::annotation; });
    if (!attr_node)
        return meta::OptionalAnnotation{};
    return meta::OptionalAnnotation{parseAnnotation(attr_node)};
}

meta::OptionalId Parser::getId(const pugi::xml_node &node) const
{
    const auto attr = node.attribute("id");
    if (!attr)
        return meta::OptionalId{};
    return meta::OptionalId(meta::Id{attr.as_string()});
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

            const fs::path s_p{working_dir_ / s->uri};
            return fs::equivalent(s_p, curr_p);
        });
    return (it != state.schemas.end()) ? *it : nullptr;
}

} // namespace cppxsd::parser
