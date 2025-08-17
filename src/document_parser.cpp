#include <document_parser.hpp>

#include <element.hpp>
#include <doctype_element.hpp>
#include <self_closing_element.hpp>
#include <iostream>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <stack>
#include <sstream>
#include <set>
#include <cctype>

#include <thread>
#include <chrono>

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
            __end_pos = std::min(__end_pos, end_pos);

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
    void remove_all_line_breaks(std::string &html)
    {
        html.erase(std::remove(html.begin(), html.end(), '\n'), html.end());
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

        attributes.erase("");
        attributes.erase("/");
        attributes.erase(" ");

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
    bool is_closing_tag(std::string &tag)
    {
        return tag.size() > 1 && tag[0] == '/' && tag[1] != '/';
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
    std::string read_tag(const std::string &html, size_t &pos)
    {
        size_t start = pos + 1;
        size_t end = html.find('>', start);
        if (end == std::string::npos)
        {
            throw std::runtime_error("Malformed HTML: no closing '>' found");
        }

        std::string tag = html.substr(start, end - start);
        pos = end; // Update position to the closing '>'
        return tag;
    }
    std::string fix_tag(std::string &tag_with_atrs)
    {
        std::string tag_name;
        for (size_t i = 0; i < tag_with_atrs.size(); ++i)
        {
            if (tag_with_atrs[i] == ' ' || tag_with_atrs[i] == '\t' || tag_with_atrs[i] == '\n')
            {
                tag_name = tag_with_atrs.substr(0, i);
                return tag_name;
            }
        }

        return tag_with_atrs;
    }
    std::vector<std::shared_ptr<element>> solve_recursive(std::string &html)
    {
        std::vector<std::shared_ptr<element>> result;

        size_t pos_of_first_opening_tag = html.find('<');
        if (pos_of_first_opening_tag == std::string::npos)
        {
            auto text_element = std::make_shared<element>("NO_TAG", html);
            result.push_back(text_element);
            return result;
        }

        do
        {
            std::string tag_content;
            for (size_t i = pos_of_first_opening_tag + 1; i < html.size(); ++i)
            {
                if (html[i] == '>')
                {
                    tag_content = html.substr(pos_of_first_opening_tag + 1, i - pos_of_first_opening_tag - 1);
                    break;
                }
            }
            size_t end_of_body = std::string::npos;

            auto [tag_name, attributes] = extract_tag_and_attributes(tag_content);
            tag_name = trim(tag_name);
            attributes = trim(attributes);
            auto parsed_attributes = parse_attributes(attributes);
            if (is_self_closing_tag(tag_name))
            {
                auto elm = std::make_shared<self_closing_element>(tag_name, parsed_attributes);
                result.push_back(elm);
                end_of_body = pos_of_first_opening_tag + tag_content.size() + 1; // +1 for '>'
            }
            else
            {
                auto opening_element = std::make_shared<element>(tag_name, parsed_attributes);

                // find it's end
                size_t body_content_begin = pos_of_first_opening_tag + tag_content.size() + 2; // +2 for '<' and '>'
                std::stack<std::string> tag_stack;
                tag_stack.push(tag_name);
                std::string body_content;
                std::string current_tag_name;
                for (size_t i = body_content_begin; i < html.size(); ++i)
                {
                    if (html[i] == '<')
                    {
                        size_t old_end = i;
                        std::string tag = read_tag(html, i);
                        tag = fix_tag(tag);
                        if (is_self_closing_tag(tag))
                        {
                            continue;
                        }
                        if (is_closing_tag(tag))
                        {
                            current_tag_name = tag.substr(1); // Remove the leading '/'
                            if (!tag_stack.empty() && tag_stack.top() == current_tag_name)
                            {
                                tag_stack.pop();
                                if (tag_stack.empty())
                                {
                                    body_content = html.substr(body_content_begin, old_end - body_content_begin);
                                    end_of_body = i;
                                    break;
                                }
                            }
                            else
                            {
                                throw std::runtime_error("Unmatched closing tag: " + current_tag_name);
                            }
                        }
                        else
                        {
                            tag_stack.push((tag));
                        }
                    }
                }

                auto res = solve_recursive(body_content);

                for (const auto &child : res)
                {
                    opening_element->add_child(child);
                }
                result.push_back(opening_element);
            }
            pos_of_first_opening_tag = html.find('<', end_of_body);
        } while (pos_of_first_opening_tag != std::string::npos);
        return result;
    }

    std::vector<std::shared_ptr<element>> parse_html_string(std::string &html)
    {

        remove_all_comments(html);
        transform_tags_to_lower_case(html);
        remove_all_line_breaks(html);
        std::string doctype;
        if (has_doctype(html))
        {
            doctype = extract_doctype(html);
        }
        std::vector<std::shared_ptr<element>> result;

        if (!doctype.empty())
        {
            doctype.pop_back();
            doctype.erase(doctype.begin(), doctype.begin() + 9); // Remove "<!doctype "
            std::shared_ptr<element> doctype_element_ptr = std::make_shared<doctype_element>(doctype);
            result.insert(result.begin(), doctype_element_ptr);
        }

        auto solved = solve_recursive(html);

        result.insert(result.end(), solved.begin(), solved.end());

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