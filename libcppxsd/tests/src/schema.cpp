#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>

TEST_CASE("schema parser")
{
    constexpr std::string_view kUri = "test.xsd";
    SECTION("simple schema")
    {
        constexpr std::string_view schema = R"(
            <xsd:schema 
                xmlns="http://www.w3.org/2001/XMLSchema"
                xmlns:xsd="http://www.w3.org/2001/XMLSchema" 
                xmlns:wsc="https://www.w3schools.com/w3shoolsschema">
            </xsd:schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 1);
        const auto schema_res = res[0];

        REQUIRE(schema_res->namespaces.size() == 3);
        REQUIRE(schema_res->namespaces[0].uri == "http://www.w3.org/2001/XMLSchema");
        REQUIRE_FALSE(schema_res->namespaces[0].prefix.has_value());

        REQUIRE(schema_res->namespaces[1].uri == "http://www.w3.org/2001/XMLSchema");
        REQUIRE(schema_res->namespaces[1].prefix.has_value());
        REQUIRE(schema_res->namespaces[1].prefix.value() == "xsd");

        REQUIRE(schema_res->namespaces[2].uri == "https://www.w3schools.com/w3shoolsschema");
        REQUIRE(schema_res->namespaces[2].prefix.has_value());
        REQUIRE(schema_res->namespaces[2].prefix.value() == "wsc");
    }
}
