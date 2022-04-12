#include <ranges>
#include <cppxsd/resolve_qname.hpp>
#include "parser/helpers.hpp"
#include "parser/keys.hpp"
namespace cppxsd
{
using namespace parser;

// clang-format off
template <typename T>
concept Named = requires(T a)
{
    { a.name } -> std::convertible_to<std::string>;
};
// clang-format on

/**
 * @brief checks if a visitable type has a given name
 *
 */
struct FindQName : public boost::static_visitor<bool>
{
    const std::string_view ns;
    FindQName(std::string_view ns)
        : ns(ns)
    {}

    template <Named T>
    bool operator()(const T &t) const
    {
        return ns.ends_with(t.name);
    }

    template <typename T>
    bool operator()(const T &) const
    {
        return false;
    }
};

/**
 * @brief returns the schema of included or imported schemas. All others MAYBE visitable types, which don't export a
 * schema are returning nullptr's
 *
 */
struct GetSchemaOp : public boost::static_visitor<std::shared_ptr<meta::schema>>
{
    using UriChecker = std::function<bool(const std::string_view)>;
    const UriChecker check_uri;
    GetSchemaOp(UriChecker &&check_uri)
        : check_uri{std::forward<UriChecker>(check_uri)}
    {}

    std::shared_ptr<meta::schema> operator()(const meta::xsd_include &inc) const
    {
        return inc.schema.lock();
    }
    std::shared_ptr<meta::schema> operator()(const meta::xsd_import &inc) const
    {
        const std::string_view inc_uri = inc.namespace_uri;
        return check_uri(inc.namespace_uri) ? inc.schema_location.lock() : nullptr;
    }

    template <typename T>
    std::shared_ptr<meta::schema> operator()(const T &) const
    {
        return nullptr;
    }
};

static meta::BuildinType resolveQNameXSD(const std::string_view qname)
{
    return value_name_to_type(qname);
}

static bool is_buildin_xsd_schema(const std::string_view schema_uri)
{
    if (!is_url(schema_uri))
        return false;
    return schema_uri.ends_with("w3.org/2001/XMLSchema");
}

/**
 * @brief this visitor only allows transformations which are legal for @see meta::qname_ref::AllRefs .
 *
 */
struct ConvertToQRef : boost::static_visitor<QNameRef::ElRef>
{

    QNameRef::ElRef operator()(const meta::simpleType &t) const
    {
        return std::cref(t);
    }

    template <typename T>
    QNameRef::ElRef operator()(const T &) const
    {
        throw std::runtime_error("invalid base type");
    }
};

QNameRef resolveQName(const meta::schema &schema, const std::string_view qname)
{
    const auto ns = get_namespace_prefix(qname);
    // 1. check the current namespace
    // if the current targetNamespace matches, try to resolve the var name with the current schema.
    if (!schema.targetNamespace.expired())
    {
        const auto tref = schema.targetNamespace.lock();
        const bool own_ns_matches = ns == kEmptyNamespace ? !tref->prefix.has_value() : tref->prefix == ns;
        if (own_ns_matches)
        {
            for (const auto &c : schema.contents)
            {
                if (boost::apply_visitor(FindQName{qname}, c))
                {
                    return QNameRef{boost::apply_visitor(ConvertToQRef(), c)};
                }
            }
        }
    }

    // 2. search in the other namespaces.
    auto ns_view = schema.namespaces | std::views::filter([ns](const std::shared_ptr<meta::xmlns_namespace> &xmlns) {
                       // when ns has no prefix, search for namespaces which don't have an prefix.
                       // when ns has a prefix, search for all namespaces with such a prefix.
                       return ns == kEmptyNamespace ? !xmlns->prefix.has_value() : xmlns->prefix == ns;
                   });

    // 3. try to resolve buildin types.
    for (const auto &x : ns_view | std::views::filter([](const auto &ns) { return is_buildin_xsd_schema(ns->uri); }))
    {
        const auto buildin_ref = resolveQNameXSD(qname);
        if (buildin_ref != BuildinType::unknown)
            return QNameRef{buildin_ref};
    }

    // search in imported or included schemas iff the searched qname isn't in the current schema or not a buildin type
    auto uri_string_view =
        ns_view | std::views::transform([](const std::shared_ptr<meta::xmlns_namespace> &xmlns) { return xmlns->uri; });
    for (const auto &inc : schema.imports)
    {
        auto schema_ptr = boost::apply_visitor(GetSchemaOp([&](const std::string_view uri) {
                                                   return std::ranges::any_of(
                                                       uri_string_view, [uri](const auto &s) { return s == uri; });
                                               }),
                                               inc);
        if (schema_ptr)
        {
            const auto schema_qname_ref = resolveQName(*schema_ptr, qname);
            if (boost::apply_visitor(IsValidQName(), schema_qname_ref.ref))
                return schema_qname_ref;
        }
    }
    return QNameRef{BuildinType::unknown};
}

} // namespace cppxsd
