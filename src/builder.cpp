#include <builder.hpp>

namespace hamza_html_builder
{
    Element::Element(const std::string &name) : name(name) {}
    void Element::add_child(const Element &child)
    {
        children.push_back(child);
    }

    Document::Document() : root("html") {}
    Element &Document::get_root() { return root; }

    Document create_document()
    {
        Document doc;
        Element &root = doc.get_root();

        Element head("head");
        head.add_child(Element("title"));
        root.add_child(head);

        Element body("body");
        body.add_child(Element("h1"));
        body.add_child(Element("p"));
        root.add_child(body);

        return doc;
    }
};