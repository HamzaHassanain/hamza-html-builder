#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace hamza_html_builder
{

    class element
    {
        std::string tag;
        std::string text_content;
        std::map<std::string, std::string> attributes;
        std::vector<std::shared_ptr<element>> children;

    public:
        element();
        element(const std::string &tag);
        element(const std::string &tag, const std::string &text_content);
        element(const std::string &tag, const std::map<std::string, std::string> &attributes);
        element(const std::string &tag, const std::string &text_content, const std::map<std::string, std::string> &attributes);

        virtual void add_child(std::shared_ptr<element> child);
        virtual void set_text_content(const std::string &text_content);

        virtual void set_params_recursive(const std::map<std::string, std::string> &params);
        virtual void set_params(const std::map<std::string, std::string> &params);
        virtual element copy() const;

        virtual std::string get_text_content() const;
        virtual std::vector<std::shared_ptr<element>> get_children() const;
        virtual std::string to_string() const;
        std::string get_tag() const;
        std::map<std::string, std::string> get_attributes() const;
        std::string get_attribute(const std::string &key) const;
    };

}