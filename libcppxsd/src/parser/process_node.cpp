#include <algorithm>
#include <iostream>
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
            std::cout << "unhandled: " << node.name() << std::endl;
    }
}
void process_file(State &state, const fs::path &file_path)
{
    const std::string file_path_str{file_path.string()};
    if (state.already_parsed.contains(file_path_str))
    {
        std::cout << file_path << " already processed" << std::endl;
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
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
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
} // namespace cppxsd::parser
