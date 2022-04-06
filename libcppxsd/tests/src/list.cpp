#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>
#include <cppxsd/resolve_qname.hpp>
#include "helpers.hpp"
namespace fs = std::filesystem;
namespace m = cppxsd::meta;

TEST_CASE("list")
{
    constexpr std::string_view kUri = "test.xsd";
    SECTION("simple list with buildin integer")
    {
        constexpr std::string_view schema = R"(
            <?xml version="1.0" encoding="utf-8"?>
            <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema" targetNamespace="myns" xmlns="myns">
                <xs:simpleType name='listOfIntegers'>
                    <xs:list itemType='xs:integer'/>
                </xs:simpleType>
            </xs:schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 1);
        const auto res_schema = res[0];
        REQUIRE(res_schema->contents.size() == 1);
        const auto &res_simpleType = res_schema->contents[0];
        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::simpleType>{}, res_simpleType));

        boost::apply_visitor(
            require_type<m::simpleType>{[&](const m::simpleType &simpleType) {
                REQUIRE(simpleType.name == "listOfIntegers");
                REQUIRE_FALSE(simpleType.annotation.has_value());
                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::list>{}, simpleType.content));
                boost::apply_visitor(
                    require_type<m::list>{[&](const m::list &l) {
                        REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::QName>{}, l.baseType));

                        boost::apply_visitor(
                            require_type<m::QName>{[&](const m::QName &qname_ref) {
                                const auto qname = cppxsd::resolveQName(res_schema, qname_ref.name);
                                REQUIRE_NOTHROW(boost::apply_visitor(require_type<m::BuildinType>{}, qname.ref));
                                boost::apply_visitor(require_type<m::BuildinType>{[](const m::BuildinType t) {
                                                         REQUIRE(t == m::BuildinType::xsd_integer);
                                                     }},
                                                     qname.ref);
                            }},
                            l.baseType);
                    }},
                    simpleType.content);
            }},
            res_simpleType);
    }
}
