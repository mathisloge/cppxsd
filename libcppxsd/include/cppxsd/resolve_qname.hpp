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
    using ElRef = boost::variant<meta::BuildinType, std::reference_wrapper<const meta::simpleType>>;
    ElRef ref;
};

QNameRef LIBCPPXSD_EXPORT resolveQName(const std::shared_ptr<meta::schema> &schema, const std::string_view qname);

} // namespace cppxsd
