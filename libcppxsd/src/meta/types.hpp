#pragma once
#include <string>
#include <variant>
#include <vector>
namespace cppxsd::meta
{

template <typename T>
struct BaseType
{};

struct Element;
struct ListElement;
struct ElementRef; // global visibility

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

struct MinMaxEl
{
    int min;
    int max;
};
using ElementType = std::variant<StringType,
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
                                 ElementRef>;

struct BaseElement
{
    std::string name;
};

struct ElementRef : public BaseElement
{
    std::string element_name;
};
struct ListElement : public BaseElement
{
    int min;
    int max;
    ElementType element;
};

using ElementChildType = std::variant<ElementType, ListElement>;
struct Element
{
    ElementRef base;
    std::vector<ElementChildType> childs;
};

struct GlobalNamespace
{
    std::vector<Element> elements;
};

} // namespace cppxsd::meta
