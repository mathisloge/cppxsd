#pragma once
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include <pugixml.hpp>

struct ParseException : public std::exception
{
    ParseException(const std::string_view current_processing_node_type,
                   const std::vector<std::string_view> &node_types,
                   const pugi::xml_node &node);
    const char *what() const noexcept override;

  private:
    const std::string msg;
};
