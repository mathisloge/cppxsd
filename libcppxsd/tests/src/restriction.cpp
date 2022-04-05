#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>
#include "helpers.hpp"
namespace fs = std::filesystem;

TEST_CASE("restriction")
{
    constexpr std::string_view kUri = "test.xsd";
    SECTION("simple restriction")
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
        REQUIRE_NOTHROW(boost::apply_visitor(require_type<cppxsd::meta::simpleType>{}, res_simpleType));

        boost::apply_visitor(
            require_type<cppxsd::meta::simpleType>{[](const cppxsd::meta::simpleType &simpleType) {
                REQUIRE(simpleType.name == "LastNameType");
                REQUIRE_FALSE(simpleType.annotation.has_value());
                REQUIRE_NOTHROW(boost::apply_visitor(require_type<cppxsd::meta::restriction>{}, simpleType.content));
            }},
            res_simpleType);
    }
}
