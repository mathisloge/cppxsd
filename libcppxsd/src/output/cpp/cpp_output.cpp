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
std::ostream& indent(int indent_val){
    return std::cout << std::setfill(' ') << std::setw(indent_val);
}
// clang-format on
template <Named T>
std::ostream &print(int n)
{
    return indent(n) << "" << T::kName << " ";
}

template <NamedRefType T>
std::ostream &print(int n)
{
    return indent(n) << "" << T::type::kName << " ";
}

struct Visit : boost::static_visitor<void>
{
    const int indent_;
    Visit(int i)
        : indent_{i}
    {}
    void operator()(const meta::schema &i) const
    {
        print<meta::schema>(indent_);
        std::cout << i.uri << std::endl;
        for (const auto &imp : i.imports)
            boost::apply_visitor(Visit(indent_ + 1), imp);

        for (const auto &imp : i.contents)
            boost::apply_visitor(Visit(indent_ + 1), imp);
    }
    void operator()(const meta::xsd_include &i) const
    {
        print<meta::xsd_include>(indent_);
        std::cout << std::endl;
        auto l = i.schema.lock();
        Visit{indent_ + 1}(*l);
    }
    void operator()(const meta::xsd_import &i) const
    {
        print<meta::xsd_import>(indent_);
        std::cout << std::endl;
        auto l = i.schema_location.lock();
        Visit{indent_ + 1}(*l);
    }
    void operator()(const meta::redefine &i) const
    {
        print<meta::redefine>(indent_) << std::endl;
    }
    void operator()(const meta::annotation &i) const
    {
        print<meta::annotation>(indent_) << std::endl;
    }
    void operator()(const meta::simpleType &i) const
    {
        print<meta::simpleType>(indent_) << " " << i.name << std::endl;
    }
    void operator()(const meta::group &i) const
    {
        print<meta::group>(indent_) << std::endl;
    }
    void operator()(const meta::attributeGroup &i) const
    {
        print<meta::attributeGroup>(indent_) << std::endl;
    }
    void operator()(const meta::element &i) const
    {
        print<meta::element>(indent_) << std::endl;
    }
    void operator()(const meta::attribute &i) const
    {
        print<meta::attribute>(indent_) << std::endl;
    }
    void operator()(const meta::notation &i) const
    {
        print<meta::notation>(indent_) << std::endl;
    }
    void operator()(const meta::xmlns_namespace &i) const
    {
        print<meta::xmlns_namespace>(indent_) << std::endl;
    }
    void operator()(const meta::list &i) const
    {
        print<meta::list>(indent_) << std::endl;
    }
    void operator()(const meta::enumeration &i) const
    {
        print<meta::enumeration>(indent_) << std::endl;
    }
    void operator()(const meta::complexType &i) const
    {
        print<meta::complexType>(indent_) << std::endl;
    }
    void operator()(const meta::simpleContent &i) const
    {
        print<meta::simpleContent>(indent_) << std::endl;
    }
    void operator()(const meta::complexContent &i) const
    {
        print<meta::complexContent>(indent_) << std::endl;
    }
    void operator()(const meta::extension &i) const
    {
        print<meta::extension>(indent_) << std::endl;
    }
    void operator()(const meta::choice &i) const
    {
        print<meta::choice>(indent_) << std::endl;
    }
    void operator()(const meta::any &i) const
    {
        print<meta::any>(indent_) << std::endl;
    }
    void operator()(const meta::all &i) const
    {
        print<meta::all>(indent_) << std::endl;
    }
    void operator()(const meta::xsd_union &i) const
    {
        print<meta::xsd_union>(indent_) << std::endl;
    }
    void operator()(const meta::GeneralAttributes &i) const
    {
        print<meta::GeneralAttributes>(indent_) << std::endl;
    }
    void operator()(const meta::anyAttribute &i) const
    {
        print<meta::anyAttribute>(indent_) << std::endl;
    }
    void operator()(const meta::documentation &i) const
    {
        print<meta::documentation>(indent_) << std::endl;
    }
    void operator()(const meta::appInfo &i) const
    {
        print<meta::appInfo>(indent_) << std::endl;
    }
    void operator()(const meta::QName &i) const
    {
        indent(indent_) << "QName: " << i.name << std::endl;
    }
};

void CppOutput::operator()(const meta::schema &i)
{
    Visit x(0);
    x(i);
}
} // namespace cppxsd::out
