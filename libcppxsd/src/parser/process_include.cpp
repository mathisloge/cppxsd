#include "exceptions.hpp"
#include "nodes.hpp"
namespace cppxsd::parser
{
void process_include(State &state, const pugi::xml_node &node)
{
    const auto attr =  require_attr(kNodeId_include, "schemaLocation", node);
    const auto org_xsd = state.current_file;

    const std::string include_uri{attr.as_string()};
    
    if (include_uri.starts_with("http://") || include_uri.starts_with("https://"))
    {
        process_url(state, include_uri);
    }
    else
    {
        const fs::path include_path{include_uri};
        if (include_path.is_relative())
            process_file(state, org_xsd.parent_path() / include_path);
        else
            process_file(state, include_path);
    }
    state.current_file = org_xsd; // restore current file; process_file changes state current file
}
} // namespace cppxsd::parser
