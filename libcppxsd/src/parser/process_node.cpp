#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include "nodes.hpp"
namespace cppxsd::parser
{
static void process_doc(State &state, const pugi::xml_document &doc)
{
    for (const auto &node : doc)
    {
        if (is_node_type(kNodeId_schema, node.name()))
            process_schema(state, node);
        else
            throw ParseException{"document", {kNodeId_schema}, node};
    }
}
void process_file(State &state, const fs::path &file_path)
{
    const std::string file_path_str{file_path.string()};
    if (state.already_parsed.contains(file_path_str))
    {
        // todo check if path is same. might contain ../ etc.
        return;
    }
    std::cout << "processing " << file_path << std::endl;
    state.current_file = file_path;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_path_str.c_str());
    if (!result)
        throw std::runtime_error("failed to parse");

    process_doc(state, doc);

    state.already_parsed.emplace(file_path_str);
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    const size_t realsize = size * nmemb;
    std::string &mem = *reinterpret_cast<std::string *>(userp);
    std::copy(static_cast<char *>(contents), static_cast<char *>(contents) + realsize, std::back_inserter(mem));
    return realsize;
}
void process_url(State &state, const std::string &file_path)
{
    if (state.already_parsed.contains(file_path))
    {
        std::cout << file_path << " already processed" << std::endl;
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

        process_doc(state, doc);

        state.already_parsed.emplace(file_path);
    }

    curl_easy_cleanup(curl);
}

meta::ElementType type_str_to_element_type(const std::string_view type_str)
{
    using NameType = std::pair<std::string_view, meta::ElementType>;
    static const std::array<NameType, 23> kBaseMapping{
        NameType{kBuildinTypeId_string, meta::StringType{}},
        NameType{kBuildinTypeId_base64Binary, meta::ByteArrayType{}},
        NameType{kBuildinTypeId_boolean, meta::BoolType{}},
        NameType{kBuildinTypeId_byte, meta::ByteType{}},
        NameType{kBuildinTypeId_decimal, meta::DoubleType{}},
        NameType{kBuildinTypeId_double, meta::DoubleType{}},
        NameType{kBuildinTypeId_date, meta::TimeDurationType{}},
        NameType{kBuildinTypeId_dateTime, meta::TimeDurationType{}},
        NameType{kBuildinTypeId_duration, meta::TimeDurationType{}},
        NameType{kBuildinTypeId_float, meta::FloatType{}},
        NameType{kBuildinTypeId_int, meta::IntType{}},
        NameType{kBuildinTypeId_integer, meta::LongType{}},
        NameType{kBuildinTypeId_negativeInteger, meta::LongType{}},
        NameType{kBuildinTypeId_nonNegativeInteger, meta::UnsignedLongType{}},
        NameType{kBuildinTypeId_nonPositiveInteger, meta::LongType{}},
        NameType{kBuildinTypeId_normalizedString, meta::StringType{}},
        NameType{kBuildinTypeId_positiveInteger, meta::UnsignedLongType{}},
        NameType{kBuildinTypeId_long, meta::LongType{}},
        NameType{kBuildinTypeId_short, meta::ShortType{}},
        NameType{kBuildinTypeId_unsignedByte, meta::UnsignedByteType{}},
        NameType{kBuildinTypeId_unsignedInt, meta::UnsignedIntType{}},
        NameType{kBuildinTypeId_unsignedLong, meta::UnsignedLongType{}},
        NameType{kBuildinTypeId_unsignedShort, meta::UnsignedShortType{}}};
    const auto it = std::find_if(kBaseMapping.begin(), kBaseMapping.end(), [&type_str](const NameType &p) {
        return is_node_type(p.first, type_str);
    });

    return (it != kBaseMapping.end()) ? it->second : meta::TypeRef{std::string{type_str}};
}

void process_node(const std::string_view curr_node,
                  const std::set<std::string_view> allowed,
                  State &state,
                  const pugi::xml_node &node)
{
    using CbPtr = void (*)(State &, const pugi::xml_node &);
    using CbPair = std::pair<std::string_view, CbPtr>;
    constexpr std::array<CbPair, 13> kCallbacks{CbPair{kNodeId_include, process_include},
                                                CbPair{kNodeId_import, process_include},
                                                CbPair{kNodeId_complexType, process_complexType},
                                                CbPair{kNodeId_group, process_group},
                                                CbPair{kNodeId_element, process_element},
                                                CbPair{kNodeId_simpleType, process_simpleType},
                                                CbPair{kNodeId_annotation, process_annotation},
                                                CbPair{kNodeId_element, process_element},
                                                CbPair{kNodeId_documentation, process_documentation},
                                                CbPair{kNodeId_restriction, process_restriction},
                                                CbPair{kNodeId_choice, process_choice},
                                                CbPair{kNodeId_complexContent, process_complexContent},
                                                CbPair{kNodeId_sequence, process_sequence}};

    if (!node)
        throw ParseException{curr_node, allowed, node};
    std::map<std::string_view, CbPtr> allowed_cbs;
    std::copy_if(kCallbacks.begin(),
                 kCallbacks.end(),
                 std::inserter(allowed_cbs, allowed_cbs.end()),
                 [&allowed](const CbPair &cb) { return allowed.contains(cb.first); });

    const auto node_name = node.name();
    const auto it = std::find_if(allowed_cbs.begin(), allowed_cbs.end(), [&node_name](const auto &cb) {
        return is_node_type(cb.first, node_name);
    });

    if (it != allowed_cbs.end())
    {
        it->second(state, node);
        if (!state.current_el_name.empty() && !state.current_el.empty())
        {
            state.global.elements.emplace(state.current_el_name, state.current_el);
            state.current_el = {};
            state.current_el_name = "";
        }
    }
    else
        throw ParseException{curr_node, allowed, node};
}

pugi::xml_attribute require_attr(const std::string_view curr_node,
                                 const std::string_view attr,
                                 const pugi::xml_node &node)
{
    const auto xml_attr = node.attribute(attr.data());
    if (!xml_attr)
        throw ParseAttrException{curr_node, attr, node};
    return xml_attr;
}

pugi::xml_node node_find_child_of(const pugi::xml_node &node, const std::vector<std::string_view> find)
{
    return node.find_child([&](const auto &n) { return is_node_type(find, n.name()); });
}
} // namespace cppxsd::parser
