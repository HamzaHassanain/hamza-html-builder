#include <document_parser.hpp>

#include <element.hpp>
#include <doctype_element.hpp>
#include <self_closing_element.hpp>

#include <algorithm>
#include <map>
#include <stdexcept>
#include <stack>
#include <sstream>
#include <set>
#include <cctype>
namespace hamza_html_builder
{
    bool has_doctype(const std::string &html)
    {
        return html.find("<!doctype") != std::string::npos;
    }

    std::string extract_doctype(std::string &html)
    {
        size_t start = html.find("<!doctype");
        if (start == std::string::npos)
            return "";

        size_t end = html.find('>', start);
        if (end == std::string::npos)
            return "";
        // remove it form the html itself
        auto doctype = html.substr(start, end - start + 1);
        html.erase(start, end - start + 1);
        return doctype;
    }

    void transform_tags_to_lower_case(std::string &html)
    {
        size_t pos = 0;
        while ((pos = html.find('<', pos)) != std::string::npos)
        {
            size_t end_pos = html.find('>', pos);
            if (end_pos == std::string::npos)
                break;

            // do not decapitalize the attribute names, classes, and so on,
            size_t __end_pos = html.find(' ', pos + 1);
            if (__end_pos == std::string::npos)
                __end_pos = end_pos;

            std::string tag = html.substr(pos + 1, __end_pos - pos - 1);
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            html.replace(pos + 1, __end_pos - pos - 1, tag);
            pos = end_pos + 1;
        }
    }

    std::string trim(std::string &str)
    {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos)
            return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }

    std::string decode_html_entities(std::string &str)
    {
        std::string result = str;

        std::map<std::string, std::string> entities = {
            {"&amp;", "&"},
            {"&lt;", "<"},
            {"&gt;", ">"},
            {"&quot;", "\""},
            {"&apos;", "'"},
            {"&nbsp;", " "}};

        for (auto &entity : entities)
        {
            size_t pos = 0;
            while ((pos = result.find(entity.first, pos)) != std::string::npos)
            {
                result.replace(pos, entity.first.length(), entity.second);
                pos += entity.second.length();
            }
        }

        return result;
    }

    std::map<std::string, std::string> parse_attributes(std::string &attr_string)
    {
        std::map<std::string, std::string> attributes;

        if (attr_string.empty())
            return attributes;
        attr_string = trim(attr_string);
        std::string current = "";
        bool did_open_an_attribute = false;

        std::string current_key = "";
        for (int i = 0; i < static_cast<int>(attr_string.size()); i++)
        {
            char c = attr_string[i];
            if (c == '=' && !did_open_an_attribute)
            {
                current_key = trim(current);
                current = "";
            }
            else
            {
                current += c;
            }
            if (c == '"')
            {
                if (did_open_an_attribute)
                {
                    did_open_an_attribute = false;
                    current.erase(current.begin());
                    current.pop_back();
                    attributes[current_key] = current;
                    current = "";
                    current_key = "";
                }
                else
                {
                    did_open_an_attribute = true;
                }
            }
            else if (!did_open_an_attribute && (c == ' ' || c == '\t' || c == '\n'))
            {
                current_key = trim(current);
                current = "";
                if (!current_key.empty())
                    attributes[current_key] = "";
                current_key = "";
            }
        }

        if (current.size())
        {
            current_key = trim(current);
            if (!current_key.empty())
            {
                attributes[current_key] = "";
            }
        }

        return attributes;
    }

    std::set<std::string> get_self_closing_tags()
    {
        return {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"};
    }

    bool is_self_closing_tag(std::string &tag)
    {
        static std::set<std::string> self_closing_tags = get_self_closing_tags();
        std::string lower_tag = tag;
        std::transform(lower_tag.begin(), lower_tag.end(), lower_tag.begin(), ::tolower);
        return self_closing_tags.find(lower_tag) != self_closing_tags.end();
    }

    std::pair<std::string, std::string> extract_tag_and_attributes(std::string &tag_content)
    {
        size_t space_pos = tag_content.find(' ');
        if (space_pos == std::string::npos)
        {
            return {tag_content, ""};
        }

        std::string tag_name = tag_content.substr(0, space_pos);
        std::string attributes = tag_content.substr(space_pos + 1);
        return {tag_name, attributes};
    }
    void remove_all_comments(std::string &html)
    {
        size_t pos = 0;
        while ((pos = html.find("<!--", pos)) != std::string::npos)
        {
            size_t end_pos = html.find("-->", pos + 4);
            if (end_pos == std::string::npos)
                throw std::runtime_error("Malformed comment: no closing tag found");
            html.erase(pos, end_pos - pos + 3);
        }
    }

    std::vector<std::shared_ptr<element>> parse_html_string(std::string &html)
    {

        remove_all_comments(html);
        transform_tags_to_lower_case(html);
        std::string doctype;
        if (has_doctype(html))
        {
            doctype = extract_doctype(html);
        }
        std::vector<std::shared_ptr<element>> result;
        std::stack<std::shared_ptr<element>> element_stack;

        size_t pos = 0;

        while (pos < html.length())
        {
            size_t tag_start = html.find('<', pos);

            if (tag_start != pos && tag_start != std::string::npos)
            {
                std::string text = html.substr(pos, tag_start - pos);
                text = trim(text);

                if (!text.empty())
                {
                    text = decode_html_entities(text);

                    if (!element_stack.empty())
                    {
                        std::string current_text = element_stack.top()->get_text_content();
                        if (!current_text.empty())
                            current_text += " ";
                        element_stack.top()->set_text_content(current_text + text);
                    }
                    else
                    {
                        auto text_element = std::make_shared<element>("span", text);
                        result.push_back(text_element);
                    }
                }
            }

            if (tag_start == std::string::npos)
            {
                if (pos < html.length())
                {
                    std::string text = html.substr(pos);
                    text = trim(text);

                    if (!text.empty())
                    {
                        text = decode_html_entities(text);

                        if (!element_stack.empty())
                        {
                            std::string current_text = element_stack.top()->get_text_content();
                            if (!current_text.empty())
                                current_text += " ";
                            element_stack.top()->set_text_content(current_text + text);
                        }
                        else
                        {
                            auto text_element = std::make_shared<element>("span", text);
                            result.push_back(text_element);
                        }
                    }
                }
                break;
            }

            size_t tag_end = html.find('>', tag_start);
            if (tag_end == std::string::npos)
            {
                throw std::runtime_error("Malformed HTML: no closing bracket found");
            }

            std::string tag_content = html.substr(tag_start + 1, tag_end - tag_start - 1);
            tag_content = trim(tag_content);

            if (tag_content.empty())
            {
                pos = tag_end + 1;
                continue;
            }

            if (tag_content[0] == '/')
            {
                std::string closing_tag = tag_content.substr(1);
                closing_tag = trim(closing_tag);
                std::transform(closing_tag.begin(), closing_tag.end(), closing_tag.begin(), ::tolower);

                while (!element_stack.empty())
                {
                    auto current_element = element_stack.top();
                    element_stack.pop();

                    std::string current_tag = current_element->get_tag();
                    std::transform(current_tag.begin(), current_tag.end(), current_tag.begin(), ::tolower);

                    if (element_stack.empty())
                    {
                        result.push_back(current_element);
                    }
                    else
                    {
                        element_stack.top()->add_child(current_element);
                    }

                    if (current_tag == closing_tag)
                        break;
                }
            }
            else
            {
                bool is_self_closing = tag_content.back() == '/' || html.substr(tag_end - 1, 2) == "/>";

                if (is_self_closing && tag_content.back() == '/')
                {
                    tag_content = tag_content.substr(0, tag_content.length() - 1);
                    tag_content = trim(tag_content);
                }

                auto tag_info = extract_tag_and_attributes(tag_content);
                std::string tag_name = tag_info.first;
                std::string attr_string = tag_info.second;

                std::transform(tag_name.begin(), tag_name.end(), tag_name.begin(), ::tolower);

                std::map<std::string, std::string> attributes = parse_attributes(attr_string);

                std::shared_ptr<element> new_element;

                if (is_self_closing || is_self_closing_tag(tag_name))
                {
                    new_element = std::make_shared<self_closing_element>(tag_name, attributes);

                    if (element_stack.empty())
                    {
                        result.push_back(new_element);
                    }
                    else
                    {
                        element_stack.top()->add_child(new_element);
                    }
                }
                else
                {
                    new_element = std::make_shared<element>(tag_name, attributes);
                    element_stack.push(new_element);
                }
            }

            pos = tag_end + 1;
        }

        while (!element_stack.empty())
        {
            auto unclosed_element = element_stack.top();
            element_stack.pop();

            if (element_stack.empty())
            {
                result.push_back(unclosed_element);
            }
            else
            {
                element_stack.top()->add_child(unclosed_element);
            }
        }

        if (!doctype.empty())
        {
            doctype.pop_back();
            doctype.erase(doctype.begin(), doctype.begin() + 9); // Remove "<!doctype "
            std::shared_ptr<element> doctype_element_ptr = std::make_shared<doctype_element>(doctype);
            result.insert(result.begin(), doctype_element_ptr);
        }
        return result;
    }

    std::string parse_html_with_params(const std::string &text, const std::map<std::string, std::string> &params)
    {
        std::string result = text;

        // Simple parameter substitution: {{param_name}}
        for (const auto &param : params)
        {
            std::string placeholder = "{{" + param.first + "}}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos)
            {
                result.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }

        return result;
    }
}