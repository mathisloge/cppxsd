#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>
#include <cppxsd/resolve_qname.hpp>
#include "helpers.hpp"
namespace fs = std::filesystem;
namespace m = cppxsd::meta;

using QRef = cppxsd::QNameRef;

template <typename ReqT>
void validate_qname(const std::shared_ptr<m::schema> &s, const std::string_view qname_str)
{
    const auto qname = cppxsd::resolveQName(s, qname_str);
    REQUIRE_NOTHROW(boost::apply_visitor(require_type<ReqT>{}, qname.ref));
};

TEST_CASE("union")
{
    constexpr std::string_view kUri = "test.xsd";
    SECTION("simple union of two simpleTypes and one integer")
    {
        constexpr std::string_view schema = R"(
            <?xml version="1.0" encoding="utf-8"?>
            <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" targetNamespace="myns" xmlns="myns">
                <xs:simpleType name="fontsize">
                    <xs:union memberTypes="fontbynumber fontbystringname xs:integer" />
                </xs:simpleType>

                <xs:simpleType name="fontbynumber">
                    <xs:restriction base="xs:positiveInteger">
                        <xs:maxInclusive value="72" />
                    </xs:restriction>
                </xs:simpleType>

                <xs:simpleType name="fontbystringname">
                    <xs:restriction base="xs:string">
                        <xs:enumeration value="small" />
                        <xs:enumeration value="medium" />
                        <xs:enumeration value="large" />
                    </xs:restriction>
                </xs:simpleType>
            </xs:schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 1);
        const auto res_schema = res[0];
        REQUIRE(res_schema->contents.size() == 3);
        const auto &res_simpleType = res_schema->contents[0];
        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::simpleType>{}, res_simpleType));

        boost::apply_visitor(
            require_type<m::simpleType>{[&](const m::simpleType &simpleType) {
                REQUIRE(simpleType.name == "fontsize");
                REQUIRE_FALSE(simpleType.annotation.has_value());
                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::xsd_union>{}, simpleType.content));
                boost::apply_visitor(require_type<m::xsd_union>{[&](const m::xsd_union &item) {
                                         REQUIRE(item.memberTypes.size() == 3);
                                         REQUIRE(item.memberTypes[0].name == "fontbynumber");
                                         REQUIRE(item.memberTypes[1].name == "fontbystringname");
                                         REQUIRE(item.memberTypes[2].name == "xs:integer");

                                         validate_qname<QRef::SimpleTypeRef>(res_schema, item.memberTypes[0].name);
                                         validate_qname<QRef::SimpleTypeRef>(res_schema, item.memberTypes[1].name);
                                         validate_qname<QRef::BuildinRef>(res_schema, item.memberTypes[2].name);
                                     }},
                                     simpleType.content);
            }},
            res_simpleType);
    }
}
