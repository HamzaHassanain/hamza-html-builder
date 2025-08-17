
#pragma once

#include <element.hpp>

namespace hamza_html_builder
{
    class doctype_element : public element
    {
    public:
        doctype_element(const std::string &doctype) : element("!DOCTYPE", doctype) {}

        std::string to_string() const override
        {
            return "<!DOCTYPE " + get_text_content() + ">";
        }
    };
}