#include "cpp_output.hpp"
#include <fstream>
#include <iostream>
#include <cppxsd/resolve_qname.hpp>
#include <fmt/args.h>
#include <fmt/format.h>
#include "struct_builder.hpp"
#define PRINT_T(out) std::cout << out << std::endl;

using namespace fmt::literals;
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

template <typename ReturnType, typename... Lambdas>
struct lambda_visitor : public boost::static_visitor<ReturnType>, public Lambdas...
{
    lambda_visitor(Lambdas... lambdas)
        : Lambdas(lambdas)...
    {}
};
template <typename ReturnType, typename... Lambdas>
lambda_visitor<ReturnType, Lambdas...> make_lambda_visitor(Lambdas... lambdas)
{
    return {lambdas...};
}
struct ProcessingState
{
    const meta::schema &schema;
};

#if 0

struct StructDef : public ObjectDef
{
    std::string name;
    std::string instance_name;
    std::vector<std::unique_ptr<ObjectDef>> types;

    void addBuildinType(const meta::BuildinType t, const std::string_view var_name)
    {
        std::unique_ptr<TypeDef> def = std::make_unique<TypeDef>();
        def->name_str = var_name;
        switch (t)
        {
        case meta::BuildinType::xsd_boolean:
            def->type_str = "bool";
            break;
        case meta::BuildinType::xsd_string:
            def->type_str = "std::string";
            break;
        };
        types.emplace_back(std::move(def));
    }

    void addSimpleTypeType(const meta::simpleType &t, const std::string_view var_name)
    {
        std::unique_ptr<TypeDef> def = std::make_unique<TypeDef>();
        def->name_str = var_name;
        def->type_str = t.name; //! FIXME: prefix with correct namespace
        types.emplace_back(std::move(def));
    }

    void addLocalSimpleType(const meta::simpleType &t, const std::string_view var_name)
    {}

    void addLocalComplexType(const meta::complexType &t, const std::string_view var_name)
    {
        std::unique_ptr<StructDef> def = std::make_unique<StructDef>();
        def->instance_name = var_name;

        const auto visitor =
            make_lambda_visitor<void>([&](const meta::simpleContent &x) {},
                                      [&](const meta::complexContent &x) {},
                                      [&](const meta::complexType::SubContent &x) {
                                          std::cout << "WIP: StructDef::addLocalComplexType::SubContent" << std::endl;
                                      });

        types.emplace_back(std::move(def));
    }

    void generateDefinitionCode(std::ostream &header) const override
    {
        header << fmt::format("struct {name} ", fmt::arg("name", name)) << '{' << std::endl;
        for (const auto &t : types)
            t->generateDefinitionCode(header);
        header << "}" << (instance_name.empty() ? "" : fmt::format(" {}", instance_name)) << ';' << std::endl;
    }

    void generateImplementationCode(std::ostream &impl) const override
    {
        for (const auto &t : types)
            t->generateDefinitionCode(impl);
    }
};

struct NamespaceDef
{
    std::string name;
    std::vector<std::unique_ptr<ObjectDef>> objects;

    void process(const ProcessingState &state, const meta::element &arg)
    {
        std::unique_ptr<StructDef> def = std::make_unique<StructDef>();
        def->name = arg.name;

        const auto visitor =
            make_lambda_visitor<void>([&](const meta::simpleType &x) { def->addLocalSimpleType(x, "content"); },
                                      [&](const meta::complexType &x) { def->addLocalComplexType(x, "content"); });
        boost::apply_visitor(visitor, arg.content);

        objects.emplace_back(std::move(def));
    }

    void process(const ProcessingState &state, const meta::simpleType &arg)
    {
        std::unique_ptr<StructDef> def = std::make_unique<StructDef>();
        def->name = arg.name;

        const auto visitor = make_lambda_visitor<void>(
            [&](const meta::restriction &x) {
                const auto qname = cppxsd::resolveQName(state.schema, x.base.name);
                const auto type_visitor = make_lambda_visitor<void>(
                    [&](const cppxsd::QNameRef::BuildinRef x) { def->addBuildinType(x, "base"); },
                    [&](const cppxsd::QNameRef::SimpleTypeRef &x) { def->addSimpleTypeType(x.get(), "base"); });
                boost::apply_visitor(type_visitor, qname.ref);
            },
            [&](const meta::list &x) {},
            [&](const meta::xsd_union &x) {});
        boost::apply_visitor(visitor, arg.content);

        objects.emplace_back(std::move(def));
    }

    void process(const ProcessingState &state, const meta::complexType &arg)
    {}

    void generateDefinitionCode(std::ostream &header) const
    {

        bool close_ns = false;
        if (!name.empty())
        {
            header << fmt::format("namespace {ns} ", fmt::arg("ns", name)) << '{' << std::endl;
            close_ns = true;
        }

        for (const auto &obj : objects)
        {
            obj->generateDefinitionCode(header);
        }

        if (close_ns)
        {
            header << "}" << std::endl;
        }
    }
    void generateImplementationCode(std::ostream &impl) const
    {
        bool close_ns = false;
        if (!name.empty())
        {
            impl << fmt::format("namespace {ns} ", fmt::arg("ns", name)) << '{' << std::endl;
            close_ns = true;
        }

        for (const auto &obj : objects)
        {
            obj->generateImplementationCode(impl);
        }

        if (close_ns)
        {
            impl << "}" << std::endl;
        }
    }
};
#endif

struct NamespaceBuilder : public NamespaceDef
{
    NamespaceBuilder(const ProcessingState &state, const meta::schema &schema)
    {
        const auto &myns = schema.targetNamespace.lock();
        name_ = myns->prefix.value_or("");

        for (const auto &c : schema.contents)
        {
            TopLevelVisitor vis(state);
            add(std::forward<ObjectDefPtr>(boost::apply_visitor(vis, c)));
        }
    }

  private:
    struct TopLevelVisitor : boost::static_visitor<ObjectDefPtr>
    {
        const ProcessingState &state_;
        TopLevelVisitor(const ProcessingState &state)
            : state_{state}
        {}

        ObjectDefPtr operator()(const meta::element &arg)
        {
            //! simple type alias when name && type. E.g.: <xs:element name="myname" type="xs:string"/>
            if (!arg.name.empty() && arg.type.has_value())
            {
                const auto type_ref = cppxsd::resolveQName(state_.schema, arg.type.value().name);
                const auto visitor = make_lambda_visitor<ObjectDefPtr>(
                    [&](const cppxsd::QNameRef::BuildinRef &x) {
                        auto alias = std::make_unique<TypeAliasDef>();
                        alias->alias_str = arg.name;
                        alias->type_str = buildintype_to_datatype(x);
                        return alias;
                    },
                    [&](const cppxsd::QNameRef::SimpleTypeRef &x) {
                        auto alias = std::make_unique<TypeAliasDef>();
                        alias->alias_str = arg.name;
                        alias->type_str = x.get().name;
                        return alias;
                    });
                return boost::apply_visitor(visitor, type_ref.ref);
            }
            else if (!arg.name.empty() && arg.substitution_group.has_value()) // type inherits from substitution_group
            {
                // need a simple resolveQName method, which just finds an element with the name in the visible tree. maybe rename current `resolveQName` to `resolveQNameStrict`
                const auto type_ref = cppxsd::resolveQName(state_.schema, arg.substitution_group.value().name);

                const auto visitor = make_lambda_visitor<std::string>(
                    [&](const cppxsd::QNameRef::BuildinRef &x) { return std::string{buildintype_to_datatype(x)}; },
                    [&](const cppxsd::QNameRef::SimpleTypeRef &x) { return x.get().name; });
                auto obj = std::make_unique<StructDef>();
                obj->name_ = arg.name;
                obj->inheritFrom(boost::apply_visitor(visitor, type_ref.ref));
                return obj;
            }
            else if (!arg.name.empty())
            {
                auto obj = std::make_unique<StructDef>();
                obj->name_ = arg.name;
                return obj;
            }

            auto obj = std::make_unique<StructDef>();
            return obj;
        };
        ObjectDefPtr operator()(const meta::simpleType &arg)
        {
            auto obj = std::make_unique<StructDef>();
            return obj;
        };
        ObjectDefPtr operator()(const meta::complexType &arg)
        {
            auto obj = std::make_unique<StructDef>();
            return obj;
        };

        template <typename T>
        ObjectDefPtr operator()(const T &)
        {
            throw std::runtime_error("not implemented");
            return nullptr;
        }
    };
};

struct FileDef
{
    std::string file_name;
    ObjectDefPtr file_namespace;

    void process(const meta::schema &schema)
    {
        ProcessingState state{schema};
        auto myns = schema.targetNamespace.lock();
        file_namespace = std::make_unique<NamespaceBuilder>(state, schema);
    }

    void generateCode() const
    {
        constexpr std::string_view kEmpty = "";
        std::ofstream header{"test.gen.hpp"};
        std::ofstream impl{"test.gen.cpp"};

        header << "#pragma once" << std::endl;
        header << "#include <string>" << std::endl;
        file_namespace->generateDefinitionCode(header);

        impl << "#include <test.gen.hpp>" << std::endl;
        file_namespace->generateImplementationCode(impl, kEmpty);
    }
};

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
    FileDef filedef;
    filedef.file_name = i.file_name; // todo
    filedef.process(i);

    filedef.generateCode();
    Visit x(0);
    x(i);
}
} // namespace cppxsd::out
