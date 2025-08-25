# Hamza HTML Builder

A C++ library for programmatically building and manipulating HTML documents with support for template parameter substitution and HTML parsing.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Quick Start](#quick-start)
- [API Documentation](#api-documentation)
  - [Namespace: hamza_html_builder](#namespace-hamza_html_builder)
  - [Classes](#classes)
    - [element](#class-element)
    - [document](#class-document)
  - [Functions](#functions)
    - [parse_html_string()](#parse_html_string)
    - [parse_html_with_params()](#parameter-substitution-functions)
- [Examples](#examples)

## Overview

The HTML Builder is a C++ library designed to simplify the creation and manipulation of HTML documents programmatically. It provides a clean object-oriented interface for building HTML elements, managing document structure, and substituting template parameters.

## Features

- 🏗️ **Element-based HTML construction** - Build HTML documents using a hierarchical element structure
- 📝 **Template parameter substitution** - Use `{{parameter}}` syntax for dynamic content replacement
- 🔍 **HTML parsing** - Parse existing HTML strings into element objects
- 🎯 **Self-closing element support** - Proper handling of void elements (img, br, hr, etc.)
- 📄 **Document management** - Complete document structure with DOCTYPE support
- 🎨 **Attribute management** - Easy setting and retrieval of HTML attributes
- 🔄 **Deep copying** - Clone element trees with all children and properties

## Quick Start

```cpp
#include "html-builder-lib.hpp"
#include <iostream>

using namespace hamza_html_builder;

int main() {
    // Create a basic HTML structure
    element root("body");
    element main("main");

    // Add content with template parameters
    main.add_child(std::make_shared<element>("h1", "{{title}}"));
    main.add_child(std::make_shared<element>("p", "{{description}}"));

    root.add_child(std::make_shared<element>(main));

    // Define parameters
    std::map<std::string, std::string> params = {
        {"title", "Welcome to My Website"},
        {"description", "This is a sample description."}
    };

    // Apply parameters and generate HTML
    root.set_text_params_recursive(params);
    std::cout << root.to_string() << std::endl;

    return 0;
}
```

## API Documentation

### Namespace: hamza_html_builder

All classes and functions are contained within the `hamza_html_builder` namespace.

### Classes

#### hamza_html_builder::element

```cpp
#include "element.hpp"

// - Purpose: Core HTML element representation with hierarchical structure support
// - Features: Dynamic attribute management, hierarchical child elements, template parameters
// - Memory: Uses shared_ptr for child management and safe element sharing
// - Key methods:
  element()                                                   // — Default constructor
  element(const std::string &tag)                            // — Constructor with tag name
  element(const std::string &tag, const std::string &text_content)  // — Constructor with tag and text
  element(const std::string &tag, const std::map<std::string, std::string> &attributes)  // — Constructor with tag and attributes
  element(const std::string &tag, const std::string &text_content, const std::map<std::string, std::string> &attributes)  // — Full constructor

  virtual void add_child(std::shared_ptr<element> child)      // — Add child element to hierarchy
  virtual void set_text_content(const std::string &text_content)  // — Set element text content
  virtual void set_params_recursive(const std::map<std::string, std::string> &params)  // — Apply parameters to element tree
  virtual void set_params(const std::map<std::string, std::string> &params)  // — Apply parameters to this element only
  virtual element copy() const                                // — Create deep copy of element and children

  virtual std::string get_text_content() const               // — Get element text content
  virtual std::vector<std::shared_ptr<element>> get_children() const  // — Get all child elements
  virtual std::string to_string() const                      // — Generate HTML string representation
  std::string get_tag() const                                 // — Get HTML tag name
  std::map<std::string, std::string> get_attributes() const  // — Get all attributes
  std::string get_attribute(const std::string &key) const    // — Get specific attribute value
```

#### hamza_html_builder::self_closing_element

```cpp
#include "self_closing_element.hpp"

// - Purpose: Specialized element for HTML void elements (img, br, hr, etc.)
// - Features: Enforces self-closing constraints, prevents children and text content
// - Inheritance: Extends element class with overridden behavior
// - Supported tags: area, base, br, col, embed, hr, img, input, link, meta, param, source, track, wbr
// - Key methods:
  self_closing_element(const std::string &tag)               // — Constructor with tag name
  self_closing_element(const std::string &tag, const std::map<std::string, std::string> &attributes)  // — Constructor with tag and attributes

  virtual void add_child(std::shared_ptr<element> child) override     // — Disabled for self-closing elements
  virtual void set_text_content(const std::string &text_content) override  // — Disabled for self-closing elements
  virtual std::string to_string() const override             // — Generate self-closing HTML syntax
  virtual std::vector<std::shared_ptr<element>> get_children() const override  // — Returns empty vector
  virtual std::string get_text_content() const override      // — Returns empty string
```

#### hamza_html_builder::doctype_element

```cpp
#include "doctype_element.hpp"

// - Purpose: Specialized element for HTML DOCTYPE declarations
// - Features: Generates proper DOCTYPE syntax instead of standard element tags
// - Usage: Typically first element in HTML documents for browser compatibility
// - Key methods:
  doctype_element(const std::string &doctype)                // — Constructor with document type
  std::string to_string() const override                     // — Generate DOCTYPE declaration (<!DOCTYPE ...>)
```

#### hamza_html_builder::document

```cpp
#include "document.hpp"

// - Purpose: Represents complete HTML document with DOCTYPE declaration
// - Features: Manages document structure, combines DOCTYPE with element hierarchy
// - Usage: Top-level container for building complete HTML documents
// - Key methods:
  document(const std::string &doctype = "html")              // — Constructor with DOCTYPE (defaults to "html")
  std::string to_string() const                              // — Generate complete HTML document string
  void add_child(std::shared_ptr<element> elem)              // — Add element to document root
```

### Functions

#### hamza_html_builder::parse_html_string

```cpp
#include "document_parser.hpp"

// - Purpose: Main entry point for parsing HTML strings into element objects
// - Features: Complete document processing with preprocessing and optimization
// - Algorithm: O(n) single-pass parsing with recursive descent
// - Processing: Comment removal, tag normalization, DOCTYPE extraction, element hierarchy construction
// - Key function:
  std::vector<std::shared_ptr<element>> parse_html_string(std::string &html)  // — Parse HTML into element objects
```

#### hamza_html_builder::parse_html_with_params

```cpp
#include "document_parser.hpp"

// - Purpose: Template-based HTML generation with parameter substitution
// - Features: Replaces {{parameter_name}} placeholders with actual values
// - Use cases: Dynamic content injection, user-specific data, configuration-driven HTML
// - Key function:
  std::string parse_html_with_params(const std::string &text, const std::map<std::string, std::string> &params)  // — Substitute template parameters
```

## Examples

### Basic Element Creation

```cpp
#include "html-builder-lib.hpp"
using namespace hamza_html_builder;

// Create a simple div with text
element div("div", "Hello World");

// Create a div with attributes
std::map<std::string, std::string> attrs = {{"class", "container"}, {"id", "main"}};
element div_with_attrs("div", "Content", attrs);

// Create nested structure
element parent("div");
parent.add_child(std::make_shared<element>("h1", "Title"));
parent.add_child(std::make_shared<element>("p", "Paragraph text"));
```

### Self-Closing Elements

```cpp
#include "html-builder-lib.hpp"
using namespace hamza_html_builder;

// Create an image element
std::map<std::string, std::string> img_attrs = {
    {"src", "image.jpg"},
    {"alt", "Description"}
};
auto img = std::make_shared<self_closing_element>("img", img_attrs);

// Create a line break
auto br = std::make_shared<self_closing_element>("br");
```

### Template Parameter Substitution

```cpp
#include "html-builder-lib.hpp"
using namespace hamza_html_builder;

// Create element with template parameters
element container("div");
container.add_child(std::make_shared<element>("h1", "{{title}}"));
container.add_child(std::make_shared<element>("p", "{{content}}"));

// Define parameters
std::map<std::string, std::string> params = {
    {"title", "Dynamic Title"},
    {"content", "This content was injected via parameters"}
};

// Apply parameters recursively
container.set_text_params_recursive(params);

// Generate HTML
std::string html = container.to_string();
```

### Document Creation

```cpp
#include "html-builder-lib.hpp"
using namespace hamza_html_builder;

// Create a complete HTML document
document doc("html");

// Add head section
auto head = std::make_shared<element>("head");
head->add_child(std::make_shared<element>("title", "My Page"));

// Add body section
auto body = std::make_shared<element>("body");
body->add_child(std::make_shared<element>("h1", "Welcome"));

// Add to document
doc.add_child(head);
doc.add_child(body);

// Generate complete HTML
std::string html = doc.to_string();
```

### HTML Parsing

```cpp
#include "html-builder-lib.hpp"
using namespace hamza_html_builder;

std::string html = "<div><h1>Title</h1><p>Content</p></div>";
auto elements = parse_html_string(html);

// Access parsed elements
for (const auto& elem : elements) {
    std::cout << elem->to_string() << std::endl;
}
```

### Element Copying

```cpp
#include "html-builder-lib.hpp"
using namespace hamza_html_builder;

// Create original element
element original("div");
original.add_child(std::make_shared<element>("h1", "{{title}}"));

// Create copy
element copy = original.copy();

// Apply different parameters to each
std::map<std::string, std::string> params1 = {{"title", "First Title"}};
std::map<std::string, std::string> params2 = {{"title", "Second Title"}};

original.set_text_params_recursive(params1);
copy.set_text_params_recursive(params2);

// Both elements now have different content
```
