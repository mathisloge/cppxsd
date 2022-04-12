#pragma once
#include <memory>
#include <ostream>
#include <vector>
#include <cppxsd/ast.hpp>
#include <cppxsd/resolve_qname.hpp>
#include <fmt/format.h>

constexpr std::string_view buildintype_to_datatype(const cppxsd::meta::BuildinType t)
{
    using BT = cppxsd::meta::BuildinType;
    switch (t)
    {
    case BT::xsd_string:
        return "std::string";
    default:
        return "// FIXME";
    }
}
struct ObjectDef
{
    virtual void generateDefinitionCode(std::ostream &header) const = 0;
    virtual void generateImplementationCode(std::ostream &impl, const std::string_view ns_prefix) const = 0;
};
using ObjectDefPtr = std::unique_ptr<ObjectDef>;

struct NamespaceDef : public ObjectDef
{
    static constexpr std::string_view kEmpty = "";
    std::string name_;
    std::vector<ObjectDefPtr> objs_;

    void generateDefinitionCode(std::ostream &header) const
    {

        bool close_ns = false;
        if (!name_.empty())
        {
            header << fmt::format("namespace {ns} ", fmt::arg("ns", name_)) << '{' << std::endl;
            close_ns = true;
        }

        for (const auto &obj : objs_)
            obj->generateDefinitionCode(header);

        if (close_ns)
            header << "}" << std::endl;
    }

    void generateImplementationCode(std::ostream &impl, const std::string_view ns_prefix) const
    {
        bool close_ns = false;
        if (!name_.empty())
        {
            impl << fmt::format("namespace {ns} ", fmt::arg("ns", name_)) << '{' << std::endl;
            close_ns = true;
        }

        for (const auto &obj : objs_)
            obj->generateImplementationCode(impl, kEmpty);

        if (close_ns)
            impl << "}" << std::endl;
    }

    void add(ObjectDefPtr &&obj)
    {
        objs_.emplace_back(std::forward<ObjectDefPtr>(obj));
    }
};

struct StructDef : public ObjectDef
{
    std::string name_;
    std::string instance_name_;
    std::string inherits_from_;
    std::vector<ObjectDefPtr> objs_;

    void inheritFrom(const std::string_view inherit_obj)
    {
        inherits_from_ = inherit_obj;
    }

    void generateDefinitionCode(std::ostream &header) const override
    {
        header << fmt::format("struct {name} {inherit}",
                              fmt::arg("name", name_),
                              fmt::arg("inherit", inherits_from_.empty() ? "" : fmt::format(": public {} ", inherits_from_)))
               << '{' << std::endl;
        for (const auto &obj : objs_)
            obj->generateDefinitionCode(header);
        header << "}" << (instance_name_.empty() ? "" : fmt::format(" {}", instance_name_)) << ';' << std::endl;
    };

    void generateImplementationCode(std::ostream &impl, const std::string_view ns_prefix) const override
    {
        const std::string my_ns_prefix = fmt::format("{}{}", ns_prefix.empty() ? "" : "::", name_);
        for (const auto &obj : objs_)
            obj->generateImplementationCode(impl, my_ns_prefix);
    };

    void add(ObjectDefPtr &&obj)
    {
        objs_.emplace_back(std::forward<ObjectDefPtr>(obj));
    }
};

struct VarDef : public ObjectDef
{
    std::string type_str;
    std::string name_str;

    void generateDefinitionCode(std::ostream &header) const override
    {
        header << type_str << ' ' << name_str << ';' << std::endl;
    }

    void generateImplementationCode(std::ostream &impl, const std::string_view ns_prefix) const override
    {}
};

struct TypeAliasDef : public ObjectDef
{
    std::string type_str;
    std::string alias_str;

    void generateDefinitionCode(std::ostream &header) const override
    {
        header << "using " << alias_str << " = " << type_str << ';' << std::endl;
    }

    void generateImplementationCode(std::ostream &impl, const std::string_view ns_prefix) const override
    {}
};
