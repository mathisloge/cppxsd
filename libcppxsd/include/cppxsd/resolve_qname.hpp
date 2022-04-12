#pragma once
#include <boost/variant.hpp>
#include "ast.hpp"
#include "libcppxsd_export.h"
namespace cppxsd
{

struct IsValidQName : public boost::static_visitor<bool>
{
    bool operator()(const meta::BuildinType &t) const
    {
        return t != meta::BuildinType::unknown;
    }
    template <typename T>
    bool operator()(const T &) const
    {
        return true;
    }
};

struct QNameRef
{
    using BuildinRef = meta::BuildinType;
    using SimpleTypeRef = std::reference_wrapper<const meta::simpleType>;
    using ElRef = boost::variant<BuildinRef, SimpleTypeRef>;
    ElRef ref;
};

QNameRef LIBCPPXSD_EXPORT resolveQName(const meta::schema &schema, const std::string_view qname);

} // namespace cppxsd
