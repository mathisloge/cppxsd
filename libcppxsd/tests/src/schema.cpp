#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <cppxsd/cppxsd.hpp>
#include "helpers.hpp"
namespace fs = std::filesystem;

TEST_CASE("schema")
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
        REQUIRE(schema_res->namespaces[0]->uri == "http://www.w3.org/2001/XMLSchema");
        REQUIRE_FALSE(schema_res->namespaces[0]->prefix.has_value());

        REQUIRE(schema_res->namespaces[1]->uri == "http://www.w3.org/2001/XMLSchema");
        REQUIRE(schema_res->namespaces[1]->prefix.has_value());
        REQUIRE(schema_res->namespaces[1]->prefix.value() == "xsd");

        REQUIRE(schema_res->namespaces[2]->uri == "https://www.w3schools.com/w3shoolsschema");
        REQUIRE(schema_res->namespaces[2]->prefix.has_value());
        REQUIRE(schema_res->namespaces[2]->prefix.value() == "wsc");
    }

    SECTION("customer schema")
    {
        const auto res = cppxsd::parse("schemas/customer.xsd");
        REQUIRE(res.size() == 1);
    }

    SECTION("simple schema with include")
    {
        constexpr std::string_view schema = R"(
            <schema>
                <include schemaLocation="schemas/customer.xsd"/>
            </schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 2);
        const auto include_schema = res[0];
        REQUIRE(fs::path{include_schema->uri} == fs::path{"schemas/customer.xsd"});
        const auto main_schema = res[1];
        REQUIRE(fs::path{main_schema->uri} == fs::path{kUri});
        REQUIRE(main_schema->imports.size() == 1);

        REQUIRE_NOTHROW(boost::apply_visitor(require_type<cppxsd::meta::xsd_include>{}, main_schema->imports[0]));
    }

    SECTION("simple schema with import")
    {
        constexpr std::string_view schema = R"(
            <schema>
                <xs:import namespace="http://www.example.com" schemaLocation="schemas/customer.xsd" />
            </schema>
        )";
        const auto res = cppxsd::parse(schema, kUri);
        REQUIRE(res.size() == 2);
        const auto import_schema = res[0];
        REQUIRE(fs::path{import_schema->uri} == fs::path{"schemas/customer.xsd"});
        const auto main_schema = res[1];
        REQUIRE(fs::path{main_schema->uri} == fs::path{kUri});
        REQUIRE(main_schema->imports.size() == 1);

        REQUIRE_NOTHROW(boost::apply_visitor(require_type<cppxsd::meta::xsd_import>{}, main_schema->imports[0]));
    }
}

/*
TEST_CASE("xsd schema")
{
    const auto res = cppxsd::parse("schemas/XMLSchema.xsd");
    REQUIRE(res.size() == 1);
}
*/
