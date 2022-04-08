#pragma once
#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <boost/variant.hpp>
namespace cppxsd::meta
{

using NameT = std::string_view;
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
struct sequence;
struct choice;
namespace attributes
{
using lang = std::string;
};

// datatypes
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
// end datatypes

enum class BuildinType
{
    xsd_duration,
    xsd_dateTime,
    xsd_time,
    xsd_date,
    xsd_gYearMonth,
    xsd_gYear,
    xsd_gMonthDay,
    xsd_gDay,
    xsd_gMonth,
    xsd_string,
    xsd_boolean,
    xsd_base64Binary,
    xsd_hexBinary,
    xsd_float,
    xsd_decimal,
    xsd_double,
    xsd_anyUri,

    // decimals
    xsd_integer,
    xsd_nonPositiveInteger,
    xsd_negativeInteger,
    xsd_long,
    xsd_int,
    xsd_short,
    xsd_byte,
    xsd_nonNegativeInteger,
    xsd_unsignedLong,
    xsd_unsignedInt,
    xsd_unsignedShort,
    xsd_unsignedByte,
    xsd_positiveInteger,

    // strings
    xsd_normalizedString,
    xsd_token,
    xsd_language,

    unknown
};

struct Id
{
    std::string id;
};
using OptionalId = std::optional<Id>;

//! https://www.w3schools.com/xml/el_appinfo.asp
struct appInfo
{
    static constexpr NameT kName = "appInfo";

    std::optional<std::string> source;
    // any other string?
};

//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-documentation
struct documentation
{
    static constexpr NameT kName = "documentation";

    std::string contents;
    attributes::lang language;
};

//! https://www.w3schools.com/xml/el_annotation.asp
struct annotation
{
    static constexpr NameT kName = "annotation";

    using Content = boost::variant<appInfo, documentation>;

    OptionalId id;
    std::vector<Content> childs;
};
using OptionalAnnotation = std::optional<annotation>;

//! https://www.w3schools.com/xml/el_notation.asp
struct notation
{
    static constexpr NameT kName = "notation";
};

struct XsdBaseElement
{
    // attributes
    OptionalId id;

    // content
    OptionalAnnotation annotation;
};

//! https://www.w3schools.com/xml/el_anyattribute.asp
struct anyAttribute
{
    static constexpr NameT kName = "anyAttribute";

    OptionalId id;
    OptionalAnnotation annotation;
};

struct GeneralAttributes
{
    static constexpr NameT kName = "generalAttributes";

    using Attr = std::vector<boost::variant<attribute, boost::recursive_wrapper<attributeGroup>>>;
    Attr attributes;
    std::optional<anyAttribute> any_attributes;
};

//! https://www.w3schools.com/xml/el_attributegroup.asp
struct attributeGroup : XsdBaseElement
{
    static constexpr NameT kName = "attributeGroup";

    GeneralAttributes attributes;
};

//! https://www.w3schools.com/xml/el_attribute.asp
struct attribute : XsdBaseElement
{
    static constexpr NameT kName = "attribute";

    std::optional<boost::recursive_wrapper<simpleType>> content;
};

struct QName
{
    std::string name;
};

//! https://www.w3schools.com/xml/el_restriction.asp
struct restriction : XsdBaseElement
{
    static constexpr NameT kName = "restriction";

    QName base;
    GeneralAttributes attributes;
};

struct xsd_union : XsdBaseElement
{
    static constexpr NameT kName = "union";

    std::vector<QName> memberTypes;
};

//! https://www.w3schools.com/xml/el_element.asp
struct element : XsdBaseElement
{
    static constexpr NameT kName = "element";

    using Content = boost::variant<boost::recursive_wrapper<simpleType>, boost::recursive_wrapper<complexType>>;
    Content content;
};

//! https://www.w3schools.com/xml/el_all.asp
struct all : XsdBaseElement
{
    static constexpr NameT kName = "all";
    uint64_t max_occurs;
    uint64_t min_occurs;

    std::vector<element> content;
};

//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-any
struct any : XsdBaseElement
{
    static constexpr NameT kName = "any";
};

struct group : XsdBaseElement
{
    static constexpr NameT kName = "group";
    QName ref;
    std::string name;

    uint64_t max_occurs;
    uint64_t min_occurs;

    using Content = boost::variant<all, boost::recursive_wrapper<choice>, boost::recursive_wrapper<sequence>>;
    Content content;
};

//! https://www.w3schools.com/xml/el_sequence.asp
//! https://www.w3schools.com/xml/el_choice.asp
struct common_sequence : XsdBaseElement
{
    using Content =
        boost::variant<element, group, boost::recursive_wrapper<choice>, boost::recursive_wrapper<sequence>, any>;
    static constexpr uint64_t kUnbounded = std::numeric_limits<uint64_t>::max();

    uint64_t max_occurs;
    uint64_t min_occurs;
    std::vector<Content> content;
};

//! https://www.w3schools.com/xml/el_sequence.asp
struct sequence : common_sequence
{
    static constexpr NameT kName = "sequence";
};

//! https://www.w3schools.com/xml/el_choice.asp
struct choice : common_sequence
{
    static constexpr NameT kName = "choice";
};

//! https://www.w3schools.com/xml/el_extension.asp
struct extension : XsdBaseElement
{
    static constexpr NameT kName = "extension";

    using Content = boost::variant<group, all, choice, sequence>;
    Content content;
    GeneralAttributes attributes;
};

struct common_content : XsdBaseElement
{
    using Content = boost::variant<restriction, extension>;
    Content content;
};

//! https://www.w3schools.com/xml/el_complexcontent.asp
struct complexContent : common_content
{
    static constexpr NameT kName = "complexContent";

    bool mixed;
};

//! https://www.w3schools.com/xml/el_simplecontent.asp
struct simpleContent : common_content
{
    static constexpr NameT kName = "simpleContent";
};

//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-complextype-globale-definit
struct complexType : XsdBaseElement
{
    static constexpr NameT kName = "complexType";
    enum class BlockType
    {
        all,
        extension,
        restriction,
        schemaDefault
    };
    // attributes
    std::string name;
    bool abstract;
    bool mixed;
    BlockType blockType;
    BlockType finalType;
    // childs
    //! (annotation?,(simpleContent|complexContent|((group|all|choice|sequence)?,((attribute|attributeGroup)*,anyAttribute?))))

    //! simpleContent|complexContent|((group|all|choice|sequence)?,((attribute|attributeGroup)*,anyAttribute?))
    using SubContent = boost::variant<group, all, choice, sequence>;
    using Content = boost::variant<simpleContent, complexContent, SubContent>;
    Content content;
    GeneralAttributes attributes; //! only present when Content != (simpleContent|complexContent)
};

//! https://www.w3schools.com/xml/el_simpletype.asp
struct simpleType : XsdBaseElement
{
    static constexpr NameT kName = "simpleType";
    // attributes
    std::string name;

    // childs
    using Content = boost::variant<restriction, boost::recursive_wrapper<list>, xsd_union>;
    Content content;
};

//! https://www.data2type.de/xml-xslt-xslfo/xml-schema/element-referenz/xs-enumeration
struct enumeration : XsdBaseElement
{
    static constexpr NameT kName = "enumeration";

    std::string value;
};

//! https://www.w3schools.com/xml/el_list.asp
struct list : XsdBaseElement
{
    static constexpr NameT kName = "list";

    boost::variant<QName, simpleType> baseType;
};

//! https://www.w3schools.com/xml/el_redefine.asp
struct redefine : XsdBaseElement
{
    static constexpr NameT kName = "redefine";

    using Content = boost::variant<simpleType, complexType, group, attributeGroup>;
    std::vector<Content> content;
};

//! https://www.w3schools.com/xml/el_include.asp
struct xsd_include
{
    static constexpr NameT kName = "include";

    OptionalId id;
    std::weak_ptr<schema> schema; //! required
};

//! https://www.w3schools.com/xml/el_import.asp
struct xsd_import
{
    static constexpr NameT kName = "import";

    OptionalId id;
    std::string namespace_uri;
    std::weak_ptr<schema> schema_location;
};

struct xmlns_namespace
{
    static constexpr NameT kName = "namespace";

    xmlns_namespace(std::optional<std::string> p, std::string uri)
        : prefix{p}
        , uri{uri}
    {}

    std::optional<std::string> prefix;
    std::string uri;
};

//! https://www.w3schools.com/xml/el_schema.asp
struct schema
{
    static constexpr NameT kName = "schema";

    std::string file_name;
    std::string uri;
    // attributes
    OptionalId id;
    std::vector<std::shared_ptr<xmlns_namespace>> namespaces;
    std::weak_ptr<xmlns_namespace> targetNamespace;
    // elements
    using ImportContent = boost::variant<xsd_include, xsd_import, redefine, annotation>;
    std::vector<ImportContent> imports;
    using Content = boost::variant<simpleType, complexType, group, attributeGroup, element, attribute, notation>;
    std::vector<Content> contents;
    std::vector<annotation> annotations;
};
} // namespace cppxsd::meta
