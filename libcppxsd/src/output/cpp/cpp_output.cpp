#include "cpp_output.hpp"
#include <iostream>
#define PRINT_T(out) std::cout << out << std::endl;
namespace cppxsd::out
{
// clang-format off
template <typename T>
concept Named = requires(T)
{
    { T::kName } -> std::convertible_to<std::string_view>;
};
template <typename T>
concept NamedRefType = requires(T)
{
    { T::type::kName } -> std::convertible_to<std::string_view>;
};
// clang-format on
template <Named T>
void print(int intend)
{
    std::cout << std::setfill(' ') << std::setw(intend) << "" << T::kName << " ";
}

template <NamedRefType T>
void print(int intend)
{
    std::cout << std::setfill(' ') << std::setw(intend) << "" << T::type::kName << " ";
}

struct Visit : boost::static_visitor<void>
{
    int intend;
    Visit(int i)
        : intend{i}
    {}
    void operator()(const meta::schema &i) const
    {
        print<meta::schema>(intend);
        std::cout << i.uri << std::endl;
        for (const auto &imp : i.imports)
            boost::apply_visitor(Visit(intend + 1), imp);
    }
    void operator()(const meta::xsd_include &i) const
    {
        print<meta::xsd_include>(intend);
        std::cout << std::endl;
        auto l = i.schema.lock();
        Visit{intend + 1}(*l);
    }
    void operator()(const meta::xsd_import &i) const
    {}
    void operator()(const meta::redefine &i) const
    {}
    void operator()(const meta::annotation &i) const
    {}
    void operator()(const meta::simpleType &i) const
    {}
    void operator()(const meta::group &i) const
    {}
    void operator()(const meta::attributeGroup &i) const
    {}
    void operator()(const meta::element &i) const
    {}
    void operator()(const meta::attribute &i) const
    {}
    void operator()(const meta::notation &i) const
    {}
    void operator()(const meta::xmlns_namespace &i) const
    {}
    void operator()(const meta::list &i) const
    {}
    void operator()(const meta::enumeration &i) const
    {}
    void operator()(const meta::complexType &i) const
    {}
    void operator()(const meta::simpleContent &i) const
    {}
    void operator()(const meta::complexContent &i) const
    {}
    void operator()(const meta::extension &i) const
    {}
    void operator()(const meta::choice &i) const
    {}
    void operator()(const meta::any &i) const
    {}
    void operator()(const meta::all &i) const
    {}
    void operator()(const meta::xsd_union &i) const
    {}
    void operator()(const meta::GeneralAttributes &i) const
    {}
    void operator()(const meta::anyAttribute &i) const
    {}
    void operator()(const meta::documentation &i) const
    {}
    void operator()(const meta::appInfo &i) const
    {}
    void operator()(const meta::QName &i) const
    {}
};

void CppOutput::operator()(const meta::schema &i)
{
    Visit x(0);
    x(i);
}
} // namespace cppxsd::out
