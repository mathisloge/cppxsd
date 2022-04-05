#pragma once
#include <chrono>
#include <optional>
#include <string>
#include <vector>
#include <boost/variant.hpp>
namespace cppxsd::meta
{
struct schema;
struct element;
struct complexType;
struct simpleType;
struct complexContent;
struct simpleContent;
struct enumeration;
struct list;
struct redefine;
struct attributeGroup;
struct attribute;
struct anyAttribute;
namespace attributes
{
using lang = std::string;
};
namespace datatypes
{
using StringType = std::string;
using BoolType = bool;
using ByteType = int8_t;
using ShortType = int16_t;
using IntType = int32_t;
using LongType = int64_t;
using UnsignedByteType = uint8_t;
using UnsignedShortType = uint16_t;
using UnsignedIntType = uint32_t;
using UnsignedLongType = uint64_t;
using FloatType = float;
using DoubleType = double;
using ByteArrayType = std::vector<uint8_t>;
using TimeDurationType = std::chrono::seconds;

struct Id
{
    std::string id;
};
}; // namespace datatypes

using OptionalId = std::optional<datatypes::Id>;

//! https://www.w3schools.com/xml/el_appinfo.asp
struct appInfo
{
    std::optional<std::string> source;
    // any other string?
};

//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-documentation
struct documentation
{
    std::string contents;
    attributes::lang language;
};

//! https://www.w3schools.com/xml/el_annotation.asp
struct annotation
{
    using Content = boost::variant<appInfo, documentation>;

    OptionalId id;
    std::vector<Content> childs;
};
using OptionalAnnotation = std::optional<annotation>;

//! https://www.w3schools.com/xml/el_notation.asp
struct notation
{};
struct XsdBaseElement
{
    OptionalAnnotation annotation;
};
//! https://www.w3schools.com/xml/el_anyattribute.asp
struct anyAttribute
{
    OptionalId id;
    OptionalAnnotation annotation;
};
struct GeneralAttributes
{
    using Attr = std::vector<boost::variant<attribute, boost::recursive_wrapper<attributeGroup>>>;
    Attr attributes;
    std::optional<anyAttribute> any_attributes;
};
//! https://www.w3schools.com/xml/el_attributegroup.asp
struct attributeGroup : XsdBaseElement
{
    GeneralAttributes attributes;
};
//! https://www.w3schools.com/xml/el_attribute.asp
struct attribute : XsdBaseElement
{
    std::optional<boost::recursive_wrapper<simpleType>> content;
};

struct xsd_union
{};

//! https://www.w3schools.com/xml/el_element.asp
struct element : XsdBaseElement
{
    using Content = boost::variant<boost::recursive_wrapper<simpleType>, boost::recursive_wrapper<complexType>>;
    Content content;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-all-ausserhalb-gruppe
struct all : XsdBaseElement
{
    std::vector<element> content;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-any
struct any : XsdBaseElement
{};

struct group
{};
struct sequence;
struct choice;
//! https://www.w3schools.com/xml/el_sequence.asp
struct sequence : XsdBaseElement
{
    using Content =
        boost::variant<element, group, boost::recursive_wrapper<choice>, boost::recursive_wrapper<sequence>, any>;
    std::vector<Content> content;
};
//! https://www.w3schools.com/xml/el_choice.asp
struct choice : XsdBaseElement
{
    using Content =
        boost::variant<element, group, boost::recursive_wrapper<choice>, boost::recursive_wrapper<sequence>, any>;
    std::vector<Content> content;
};
//! https://www.w3schools.com/xml/el_restriction.asp
struct restriction : XsdBaseElement
{
    GeneralAttributes attributes;
    // simpleType:

    // simpleContent

    // complexContent
    using elComplexContent = boost::variant<group, all, choice, sequence>;
    using elComp = boost::variant<elComplexContent>;
    elComp content;
};
//! https://www.w3schools.com/xml/el_extension.asp
struct extension : XsdBaseElement
{
    using Content = boost::variant<group, all, choice, sequence>;
    Content content;
    GeneralAttributes attributes;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-complexcontent
struct complexContent : XsdBaseElement
{
    using Content = boost::variant<restriction, extension>;
    Content content;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-simplecontent
struct simpleContent : XsdBaseElement
{
    using Content = boost::variant<restriction, extension>;
    Content content;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-complextype-globale-definit
struct complexType : XsdBaseElement
{
    using Content = boost::variant<simpleContent, complexContent, group, all, choice, sequence>;
    GeneralAttributes attributes;
    Content content;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-simpletype-globale-definiti
struct simpleType : XsdBaseElement
{
    using Content = boost::variant<restriction, boost::recursive_wrapper<list>, xsd_union>;
    Content content;
};
//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-enumeration
struct enumeration : XsdBaseElement
{
    std::string value;
};
//! https://www.w3schools.com/xml/el_list.asp
struct list : XsdBaseElement
{
    std::optional<simpleType> simple_type;
};
//! https://www.w3schools.com/xml/el_redefine.asp
struct redefine : XsdBaseElement
{
    using Content = boost::variant<simpleType, complexType, group, attributeGroup>;
    std::vector<Content> content;
};

//! https://www.w3schools.com/xml/el_include.asp
struct xsd_include
{
    OptionalId id;
    std::weak_ptr<schema> schema; //! required
};

//! https://www.w3schools.com/xml/el_import.asp
struct xsd_import
{
    OptionalId id;
    std::string namespace_uri;
    std::weak_ptr<schema> schema_location;
};

struct xmlns_namespace
{
    std::optional<std::string> prefix;
    std::string uri;
};
//! https://www.w3schools.com/xml/el_schema.asp
struct schema
{
    std::string file_name;
    std::string uri;
    // attributes
    OptionalId id;
    std::vector<xmlns_namespace> namespaces;
    // elements
    using ImportContent = boost::variant<xsd_include, xsd_import, redefine, annotation>;
    std::vector<ImportContent> imports;
    using Content = boost::variant<simpleType, complexType, group, attributeGroup, element, attribute, notation>;
    std::vector<Content> contents;
    std::vector<annotation> annotations;
};
} // namespace cppxsd::meta
