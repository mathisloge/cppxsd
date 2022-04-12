#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>
#include <cppxsd/resolve_qname.hpp>
#include "helpers.hpp"
namespace fs = std::filesystem;
namespace m = cppxsd::meta;

using QRef = cppxsd::QNameRef;

//! https://www.w3schools.com/xml/el_restriction.asp
TEST_CASE("restriction")
{
    constexpr std::string_view kUri = "test.xsd";

    SECTION("restriction in simpleType")
    {
        constexpr std::string_view schema = R"(
            <schema xmlns="http://www.w3.org/2001/XMLSchema">
                <simpleType name="LastNameType">
                    <restriction base="string">
                        <maxLength value="20" />
                    </restriction>
                </simpleType>
            </schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 1);
        const auto res_schema = res[0];
        REQUIRE(res_schema->contents.size() == 1);
        const auto &res_simpleType = res_schema->contents[0];
        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::simpleType>{}, res_simpleType));
        boost::apply_visitor(
            require_type<m::simpleType>{[&](const m::simpleType &simpleType) {
                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::restriction>{}, simpleType.content));

                boost::apply_visitor(
                    require_type<m::restriction>{[&](const m::restriction &restr) {
                        REQUIRE(restr.base.name == "string");
                        const auto qname = cppxsd::resolveQNameStrict(*res_schema, restr.base.name);
                        REQUIRE_NOTHROW(boost::apply_visitor(require_type<QRef::BuildinRef>{}, qname.ref));
                        boost::apply_visitor(require_type<QRef::BuildinRef>{[](const QRef::BuildinRef t) {
                                                 REQUIRE(t == QRef::BuildinRef::xsd_string);
                                             }},
                                             qname.ref);
                    }},
                    simpleType.content);
            }},
            res_simpleType);
    }

    SECTION("restriction from non buildin type")
    {
        constexpr std::string_view schema = R"(
            <schema xmlns="http://www.w3.org/2001/XMLSchema" xmlns:myns="myns" targetNamespace="myns">
                <simpleType name="NameType">
                    <restriction base="string">
                        <xs:minLength value="5"/>
                    </restriction>
                </simpleType>
                <simpleType name="LastNameType">
                    <restriction base="myns:NameType">
                        <maxLength value="20" />
                    </restriction>
                </simpleType>
            </schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 1);
        const auto res_schema = res[0];
        REQUIRE(res_schema->contents.size() == 2);
        const auto &last_name = res_schema->contents[1];

        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::simpleType>{}, last_name));
        boost::apply_visitor(
            require_type<m::simpleType>{[&](const m::simpleType &simpleType) {
                REQUIRE(simpleType.name == "LastNameType");
                REQUIRE_FALSE(simpleType.annotation.has_value());
                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::restriction>{}, simpleType.content));
                boost::apply_visitor(
                    require_type<m::restriction>{[&](const m::restriction &restr) {
                        REQUIRE(restr.base.name == "myns:NameType");
                        const auto qname = cppxsd::resolveQNameStrict(*res_schema, restr.base.name);
                        REQUIRE_NOTHROW(boost::apply_visitor(require_type<QRef::SimpleTypeRef>{}, qname.ref));
                        boost::apply_visitor(require_type<QRef::SimpleTypeRef>{[](const QRef::SimpleTypeRef &t) {
                                                 REQUIRE(t.get().name == "NameType");
                                             }},
                                             qname.ref);
                    }},
                    simpleType.content);
            }},
            last_name);
    }
}
