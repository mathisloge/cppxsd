#pragma once
#include <exception>
#include <set>
#include <string>
#include <string_view>
#include <pugixml.hpp>

struct ParseException : public std::exception
{
    ParseException(const std::string_view current_processing_node_type,
                   const std::set<std::string_view> &node_types,
                   const pugi::xml_node &node);
    const char *what() const noexcept override;

  private:
    const std::string msg;
};

struct ParseAttrException : public std::exception
{
    ParseAttrException(const std::string_view current_processing_node_type,
                       const std::string_view attr,
                       const pugi::xml_node &node);
    const char *what() const noexcept override;

  private:
    const std::string msg;
};

struct ValidationException : public std::exception
{
    ValidationException(const std::string_view curr_node, const pugi::xml_node &node, const std::string_view err_str);
    const char *what() const noexcept override;

  private:
    const std::string msg;
};
