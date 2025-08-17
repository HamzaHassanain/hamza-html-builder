#include <element.hpp>
#include <stdexcept>
namespace hamza_html_builder
{
    element::element() : tag("div") {}

    element::element(const std::string &tag) : tag(tag) {}

    element::element(const std::string &tag, const std::string &text_content)
        : tag(tag), text_content(text_content) {}

    element::element(const std::string &tag, const std::map<std::string, std::string> &attributes)
        : tag(tag), attributes(attributes) {}

    element::element(const std::string &tag, const std::string &text_content, const std::map<std::string, std::string> &attributes)
        : tag(tag), text_content(text_content), attributes(attributes) {}

    void element::add_child(std::shared_ptr<element> child)
    {
        children.push_back(child);
    }

    void element::set_text_content(const std::string &text_content)
    {
        this->text_content = text_content;
    }

    void element::set_text_content(const std::string &text_content, const std::map<std::string, std::string> &params)
    {
        // still needs a string parser,
        throw std::runtime_error("String parsing not implemented");
    }

    std::string element::get_tag() const
    {
        return tag;
    }

    std::string element::get_text_content() const
    {
        return text_content;
    }

    std::map<std::string, std::string> element::get_attributes() const
    {
        return attributes;
    }

    std::string element::get_attribute(const std::string &key) const
    {
        auto it = attributes.find(key);
        if (it != attributes.end())
        {
            return it->second;
        }
        return "";
    }

    std::vector<std::shared_ptr<element>> element::get_children() const
    {
        return children;
    }

    std::string element::to_string() const
    {
        std::string result = "<" + tag;
        for (const auto &attr : attributes)
        {
            if (!attr.second.empty())
            {
                result += " " + attr.first + "\"";
            }
            else
            {

                result += " " + attr.first + "=\"" + attr.second + "\"";
            }
        }
        result += ">" + text_content;
        for (const auto &child : children)
        {
            result += child->to_string();
        }
        result += "</" + tag + ">";
        return result;
    }

};