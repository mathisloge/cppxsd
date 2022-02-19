#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/variant.hpp>
namespace cppxsd::meta
{

template <typename T>
struct BaseType
{};

struct Element;
struct ListElement;
struct ElementRef; // global visibility
struct TypeDefinition;

using StringType = BaseType<std::string>;
using BoolType = BaseType<bool>;
using ByteType = BaseType<int8_t>;
using ShortType = BaseType<int16_t>;
using IntType = BaseType<int32_t>;
using LongType = BaseType<int64_t>;
using UnsignedByteType = BaseType<uint8_t>;
using UnsignedShortType = BaseType<uint16_t>;
using UnsignedIntType = BaseType<uint32_t>;
using UnsignedLongType = BaseType<uint64_t>;
using FloatType = BaseType<float>;
using DoubleType = BaseType<double>;
using UnsupportedBuildinType = BaseType<void *>;
using ElementType = boost::variant<UnsupportedBuildinType,
                                   StringType,
                                   BoolType,
                                   ByteType,
                                   ShortType,
                                   IntType,
                                   LongType,
                                   UnsignedByteType,
                                   UnsignedShortType,
                                   UnsignedIntType,
                                   UnsignedLongType,
                                   FloatType,
                                   DoubleType,
                                   ElementRef,
                                   boost::recursive_wrapper<ListElement>,
                                   boost::recursive_wrapper<TypeDefinition>,
                                   boost::recursive_wrapper<Element>>;

struct BaseElement
{
    std::string name;
};

struct ElementRef : public BaseElement
{};
struct ListElement : public BaseElement
{
    int min;
    int max;
    ElementType element;
};

struct TypeDefinition : public BaseElement
{
    ElementType element;
};

using VarName = std::string;
struct Element
{
    ElementRef base;
    std::unordered_map<VarName, ElementType> childs;
};

struct GlobalNamespace
{
    std::unordered_map<VarName, ElementType> elements;
};

} // namespace cppxsd::meta
