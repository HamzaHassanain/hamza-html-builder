#pragma once

#include <element.hpp>

namespace hamza_html_builder
{
    class self_closing_element : public element
    {
    public:
        self_closing_element(const std::string &tag);
        self_closing_element(const std::string &tag, const std::map<std::string, std::string> &attributes);
        virtual void add_child(std::shared_ptr<element> child) override;
        virtual std::string to_string() const override;
        virtual std::vector<std::shared_ptr<element>> get_children() const override;
        virtual std::string get_text_content() const override;
        virtual void set_text_content(const std::string &text_content) override;
    };
}