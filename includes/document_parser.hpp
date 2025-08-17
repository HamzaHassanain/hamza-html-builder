#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <element.hpp>
#include <self_closing_element.hpp>

namespace hamza_html_builder
{
    std::vector<std::shared_ptr<element>> parse_html_string(std::string &html);
    std::string parse_html_with_params(const std::string &text, const std::map<std::string, std::string> &params);
}