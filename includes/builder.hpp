#pragma once

#include <string>
#include <vector>

namespace hamza_html_builder
{
    class Element
    {
        std::string name;
        std::vector<Element> children;

    public:
        Element(const std::string &name);
        void add_child(const Element &child);
    };

    class Document
    {
        Element root;

    public:
        Document();
        Element &get_root();
    };

    Document create_document();
};