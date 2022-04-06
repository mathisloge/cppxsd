#include <filesystem>
#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>
#include "helpers.hpp"
namespace fs = std::filesystem;
namespace m = cppxsd::meta;

static void test(const m::simpleType &s)
{
    std::cout << "TEST" << s.name << std::endl;
}
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
        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::ptr<m::simpleType>>{}, res_simpleType));
        boost::apply_visitor(
            require_type<m::ptr<m::simpleType>>{[](const m::ptr<m::simpleType> &simpleType) {
                REQUIRE(simpleType->name == "LastNameType");
                REQUIRE_FALSE(simpleType->annotation.has_value());
                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::restriction>{[](const m::restriction &restr) {
                                                         REQUIRE(restr.base.base == "string");
                                                         REQUIRE_NOTHROW(boost::apply_visitor(
                                                             require_type<m::BuildinType>{[](const m::BuildinType t) {
                                                                 REQUIRE(t == m::BuildinType::xsd_string);
                                                             }},
                                                             restr.base.ref));
                                                     }},
                                                     simpleType->content));

                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::restriction>{}, simpleType->content));
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

        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::ptr<m::simpleType>>{}, last_name));
        boost::apply_visitor(
            require_type<m::ptr<m::simpleType>>{[](const m::ptr<m::simpleType> &simpleType) {
                REQUIRE(simpleType->name == "LastNameType");
                REQUIRE_FALSE(simpleType->annotation.has_value());
                REQUIRE_NOTHROW(
                    boost::apply_visitor(require_type<m::restriction>{[](const m::restriction &restr) {
                                             REQUIRE(restr.base.base == "myns:NameType");
                                             REQUIRE_NOTHROW(boost::apply_visitor(
                                                 require_type<m::qname_ref::SimpleTypePtr>{
                                                     [](const m::qname_ref::SimpleTypePtr &t) { test(*t); }},
                                                 restr.base.ref));
                                         }},
                                         simpleType->content));

                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::restriction>{}, simpleType->content));
            }},
            last_name);
    }
}
