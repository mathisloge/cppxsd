#pragma once
#include <boost/variant.hpp>
#include "ast.hpp"
#include "libcppxsd_export.h"
namespace cppxsd
{



struct QNameRef
{
    using BuildinRef = meta::BuildinType;
    using SimpleTypeRef = std::reference_wrapper<const meta::simpleType>;
    using ElRef = boost::variant<BuildinRef, SimpleTypeRef>;
    ElRef ref;
};

struct QNameAnyRef
{
    bool resolved;
    std::string name;
};


//! wrong... qname can reference any node which can be named. I guess this should be an templated function to get only the needed qnames. 
//! maybe add a laxed version which don't throws but instead returns an optional object or make strict directly "lax"
QNameRef LIBCPPXSD_EXPORT resolveQNameStrict(const meta::schema &schema, const std::string_view qname);
QNameAnyRef LIBCPPXSD_EXPORT resolveQName(const meta::schema &schema, const std::string_view qname);

} // namespace cppxsd
