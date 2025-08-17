# Hamza HTML Builder

A C++ library for programmatically building and manipulating HTML documents with support for template parameter substitution and HTML parsing.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Quick Start](#quick-start)
- [About CMakeLists](#about-cmakelists)
- [API Documentation](#api-documentation)
  - [Namespace: hamza_html_builder](#namespace-hamza_html_builder)
  - [Classes](#classes)
    - [element](#class-element)
    - [document](#class-document)
  - [Functions](#functions)
    - [parse_html_string()](#parse_html_string)
    - [parse_html_with_params()](#parameter-substitution-functions)
- [Examples](#examples)
- [Building the Project](#building-the-project)
- [Project Structure](#project-structure)

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
#include <iostream>
#include <element.hpp>
#include <document_parser.hpp>
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

## About CMakeLists

The project uses CMake for build configuration, allowing easy integration with various build systems. The `CMakeLists.txt` file defines the project structure, dependencies, and build targets.

### CMakeLists.txt Detailed Explanation

Let's break down every single line and section of the CMakeLists.txt file:

#### 1. Project Setup and Minimum Version

```cmake
cmake_minimum_required(VERSION 3.10)
project(html_builder)
```

- **`cmake_minimum_required(VERSION 3.10)`**: Specifies that CMake version 3.10 or higher is required to build this project. This ensures compatibility with modern CMake features.
- **`project(html_builder)`**: Defines the project name as "html_builder". This sets the `PROJECT_NAME` variable and enables language support (C/C++ by default).

#### 2. Environment File Loading Function

```cmake
function(load_env_file env_file)
    if(EXISTS ${env_file})
        file(READ ${env_file} ENV_CONTENT)
        string(REPLACE "\n" ";" ENV_LINES ${ENV_CONTENT})

        foreach(line ${ENV_LINES})
            # Skip empty lines and comments
            if(line AND NOT line MATCHES "^#")
                string(FIND ${line} "=" eq_pos)
                if(eq_pos GREATER -1)
                    string(SUBSTRING ${line} 0 ${eq_pos} var_name)
                    math(EXPR val_start "${eq_pos} + 1")
                    string(SUBSTRING ${line} ${val_start} -1 var_value)

                    # Remove any whitespace
                    string(STRIP ${var_name} var_name)
                    string(STRIP ${var_value} var_value)

                    # Set the variable in parent scope
                    set(${var_name} ${var_value} PARENT_SCOPE)
                    message(STATUS "Loaded env variable: ${var_name}=${var_value}")
                endif()
            endif()
        endforeach()
    else()
        message(WARNING ".env file not found at ${env_file}")
    endif()
endfunction()
```

This is a custom function that implements `.env` file support in CMake:

- **`function(load_env_file env_file)`**: Defines a function that takes an environment file path as parameter
- **`if(EXISTS ${env_file})`**: Checks if the .env file exists before trying to read it
- **`file(READ ${env_file} ENV_CONTENT)`**: Reads the entire .env file content into the `ENV_CONTENT` variable
- **`string(REPLACE "\n" ";" ENV_LINES ${ENV_CONTENT})`**: Converts newlines to semicolons to create a CMake list of lines
- **`foreach(line ${ENV_LINES})`**: Iterates through each line in the file
- **`if(line AND NOT line MATCHES "^#")`**: Skips empty lines and comment lines (starting with #)
- **`string(FIND ${line} "=" eq_pos)`**: Finds the position of the equals sign to separate key from value
- **`string(SUBSTRING)`**: Extracts the variable name (before =) and value (after =)
- **`string(STRIP)`**: Removes leading/trailing whitespace from both key and value
- **`set(${var_name} ${var_value} PARENT_SCOPE)`**: Sets the variable in the parent scope so it's available outside the function
- **`message(STATUS "...")`**: Prints a status message showing which variables were loaded
- **`message(WARNING "...")`**: Warns if the .env file is not found

#### 3. Loading Environment Variables

```cmake
load_env_file(${CMAKE_SOURCE_DIR}/.env)
```

- **`${CMAKE_SOURCE_DIR}`**: CMake variable containing the path to the top-level source directory
- **`load_env_file(...)`**: Calls the custom function to load variables from `.env` file in the project root

#### 4. Debug Mode Configuration

```cmake
set(DEBUG_MODE ON CACHE BOOL "Enable Debug Mode")

if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE)
endif()
```

- **`set(DEBUG_MODE ON CACHE BOOL "Enable Debug Mode")`**:
  - Creates a cached CMake variable `DEBUG_MODE` with default value `ON`
  - `CACHE BOOL` makes it a boolean cache variable that can be modified via CMake GUI or command line
  - The description "Enable Debug Mode" appears in CMake GUI
- **`if(DEBUG_MODE)`**: Conditional block that executes when DEBUG_MODE is true
- **`add_definitions(-DDEBUG_MODE)`**: Adds `-DDEBUG_MODE` preprocessor definition to all targets, making `DEBUG_MODE` macro available in C++ code

#### 5. Compiler Flags Configuration

```cmake
if(LOCAL_TEST AND LOCAL_TEST STREQUAL "1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fsanitize=address -g -O0")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
```

This section configures different compiler flags based on the `LOCAL_TEST` environment variable:

- **`if(LOCAL_TEST AND LOCAL_TEST STREQUAL "1")`**: Checks if LOCAL_TEST variable exists and equals "1"
- **Development/Testing Flags** (when LOCAL_TEST=1):
  - **`-Wall`**: Enables most warning messages
  - **`-Wextra`**: Enables extra warning messages beyond -Wall
  - **`-pedantic`**: Enables strict ISO C++ compliance warnings
  - **`-fsanitize=address`**: Enables AddressSanitizer for detecting memory errors (buffer overflows, use-after-free, etc.)
  - **`-g`**: Includes debugging information in the binary
  - **`-O0`**: Disables optimization for easier debugging
- **Production Flags** (when LOCAL_TEST≠1):
  - **`"${CMAKE_CXX_FLAGS}"`**: Keeps default CMake flags without modifications

#### 6. Source File Collection

```cmake
file(GLOB SRC_FILES src/*.cpp)
```

- **`file(GLOB SRC_FILES src/*.cpp)`**:
  - Uses glob pattern to automatically find all `.cpp` files in the `src/` directory
  - Stores the list of found files in the `SRC_FILES` variable
  - This automatically includes any new .cpp files added to src/ without manually updating CMakeLists.txt

#### 7. C++ Standard and Include Directories

```cmake
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/includes)
```

- **`set(CMAKE_CXX_STANDARD 17)`**: Sets the C++ standard to C++17 for all targets in this project
- **`include_directories(${CMAKE_SOURCE_DIR}/includes)`**:
  - Adds the `includes/` directory to the include path
  - This allows source files to include headers using `#include <header.hpp>` syntax
  - `${CMAKE_SOURCE_DIR}` ensures the path is relative to the project root

#### 8. Target Configuration (Conditional Build)

```cmake
if(LOCAL_TEST AND LOCAL_TEST STREQUAL "1")
    add_executable(html_builder app.cpp ${SRC_FILES})
else()
    add_library(html_builder STATIC ${SRC_FILES})
endif()
```

This section creates different build targets based on the LOCAL_TEST variable:

- **Development/Testing Mode** (when LOCAL_TEST=1):

  - **`add_executable(html_builder app.cpp ${SRC_FILES})`**:
    - Creates an executable target named "html_builder"
    - Includes `app.cpp` (main application) and all source files from `src/`
    - This builds a complete application for testing purposes

- **Library Mode** (when LOCAL_TEST≠1):
  - **`add_library(html_builder STATIC ${SRC_FILES})`**:
    - Creates a static library target named "html_builder"
    - Only includes source files from `src/` (excludes app.cpp)
    - This builds a reusable library that can be linked into other projects
    - `STATIC` means the library is statically linked (code included in final binary)

### Build Behavior Summary

The CMakeLists.txt creates a flexible build system that can operate in two modes:

1. **Development Mode** (`LOCAL_TEST=1`):

   - Builds an executable with debugging flags
   - Includes memory sanitizers and extensive warnings
   - Perfect for development and testing

2. **Library Mode** (default):
   - Builds a static library without the test application
   - Uses standard optimization flags
   - Suitable for integration into other projects

This dual-mode approach allows the same codebase to serve both as a standalone application for development and as a reusable library for production use.

## API Documentation

### Namespace: hamza_html_builder

All classes and functions are contained within the `hamza_html_builder` namespace.

### Classes

#### class element

The core class for representing HTML elements.

**Constructors:**

- `element()` - Creates an empty element
- `element(const std::string &tag)` - Creates an element with specified tag
- `element(const std::string &tag, const std::string &text_content)` - Creates an element with tag and text content
- `element(const std::string &tag, const std::map<std::string, std::string> &attributes)` - Creates an element with tag and attributes
- `element(const std::string &tag, const std::string &text_content, const std::map<std::string, std::string> &attributes)` - Creates an element with tag, text content, and attributes

**Public Methods:**

##### Element Manipulation

- `virtual void add_child(std::shared_ptr<element> child)` - Adds a child element
- `virtual element copy() const` - Creates a deep copy of the element and all its children

##### Text Content Management

- `virtual void set_text_content(const std::string &text_content)` - Sets the text content
- `virtual void set_text_content(const std::string &text_content, const std::map<std::string, std::string> &params)` - Sets text content with parameter substitution
- `virtual std::string get_text_content() const` - Returns the text content

##### Parameter Substitution

- `virtual void set_text_params_recursive(const std::map<std::string, std::string> &params)` - Applies parameters to this element and all children recursively
- `virtual void set_text_params(const std::map<std::string, std::string> &params)` - Applies parameters to this element only

##### Getters

- `std::string get_tag() const` - Returns the HTML tag name
- `std::map<std::string, std::string> get_attributes() const` - Returns all attributes
- `std::string get_attribute(const std::string &key) const` - Returns a specific attribute value
- `virtual std::vector<std::shared_ptr<element>> get_children() const` - Returns all child elements

##### Output Generation

- `virtual std::string to_string() const` - Generates the HTML string representation

#### class self_closing_element : public element

Specialized element class for self-closing HTML tags (void elements).

**Constructors:**

- `self_closing_element(const std::string &tag)` - Creates a self-closing element with specified tag
- `self_closing_element(const std::string &tag, const std::map<std::string, std::string> &attributes)` - Creates a self-closing element with tag and attributes

**Overridden Methods:**

- `virtual void add_child(std::shared_ptr<element> child) override` - Disabled for self-closing elements
- `virtual std::string to_string() const override` - Generates self-closing HTML syntax (`<tag />`)
- `virtual std::vector<std::shared_ptr<element>> get_children() const override` - Returns empty vector
- `virtual std::string get_text_content() const override` - Returns empty string
- `virtual void set_text_content(const std::string &text_content) override` - Disabled for self-closing elements
- `virtual void set_text_content(const std::string &text_content, const std::map<std::string, std::string> &params) override` - Disabled for self-closing elements

**Supported Self-Closing Tags:**

- `area`, `base`, `br`, `col`, `embed`, `hr`, `img`, `input`, `link`, `meta`, `param`, `source`, `track`, `wbr`

#### class document

Represents a complete HTML document with DOCTYPE declaration.

**Constructor:**

- `document(const std::string &doctype = "html")` - Creates a document with specified DOCTYPE (defaults to "html")

**Public Methods:**

- `std::string to_string() const` - Generates the complete HTML document string with DOCTYPE
- `void add_child(std::shared_ptr<element> elem)` - Adds an element to the document root

#### class doctype_element : public element

Specialized element for DOCTYPE declarations.

**Constructor:**

- `doctype_element(const std::string &doctype)` - Creates a DOCTYPE element

**Overridden Methods:**

- `std::string to_string() const override` - Generates DOCTYPE declaration (`<!DOCTYPE type>`)

### Functions

The document parser contains a comprehensive set of functions for HTML parsing and manipulation. Here's a detailed breakdown of each function and how they work together:

#### These are the only public functions available for HTML parsing

```cpp
using namespace hamza_html_builder;
std::vector<std::shared_ptr<element>> parse_html_string(std::string &html);
std::string parse_html_with_params(const std::string &html, const std::map<std::string, std::string> &params);
```

#### Main Parsing Functions

### Parameter Substitution Functions

```cpp
std::string parse_html_with_params(const std::string &text, const std::map<std::string, std::string> &params)
```

Substitutes template parameters in text using `{{parameter_name}}` syntax.

**Parameters:**

- `text` - Text containing parameter placeholders
- `params` - Map of parameter names to their replacement values

**Returns:** Text with all parameters substituted

### parse_html_string()

```cpp
std::vector<std::shared_ptr<element>> parse_html_string(std::string &html)
```

**Purpose:** Main entry point for HTML parsing that orchestrates the entire parsing process.

**Process Flow:**

1. **Comment Removal:** Calls `remove_all_comments()` to strip HTML comments
2. **Tag Normalization:** Uses `transform_tags_to_lower_case()` for consistent tag handling
3. **Line Break Cleanup:** Applies `remove_all_line_breaks()` for easier parsing
4. **DOCTYPE Processing:** Checks and extracts DOCTYPE with `has_doctype()` and `extract_doctype()`
5. **Recursive Parsing:** Delegates main parsing to `solve_recursive()`
6. **Result Assembly:** Combines DOCTYPE and parsed elements into final result

**Parameters:**

- `html` - Reference to HTML string (modified during parsing)

**Returns:** Vector of shared pointers to parsed elements

**Complexity:** O(n²) due to the recursive parsing algorithm

Each function below contributes to the overall parsing process, but they are inaccessible from outside the library.

##### solve_recursive()

```cpp
std::vector<std::shared_ptr<element>> solve_recursive(std::string &html)
```

**Purpose:** Core recursive parsing engine that builds the element tree structure.

**Algorithm Overview:**

1. **Base Case:** If no opening tags found, creates text element with remaining content
2. **Tag Processing Loop:** For each opening tag found:
   - Extracts tag content between `<` and `>`
   - Separates tag name from attributes using `extract_tag_and_attributes()`
   - Determines if tag is self-closing with `is_self_closing_tag()`
   - **Self-Closing Elements:** Creates `self_closing_element` instance
   - **Regular Elements:** Uses stack-based algorithm to find matching closing tag
3. **Nested Content:** Recursively processes content between opening and closing tags
4. **Stack Management:** Uses `std::stack<std::string>` to handle nested tag structures

**Performance Note:** The function has O(n²) complexity and is marked for optimization in future versions.

#### Utility Functions for HTML Processing

##### remove_all_comments()

```cpp
void remove_all_comments(std::string &html)
```

**Purpose:** Removes HTML comments (`<!-- ... -->`) from the input string.

**Algorithm:**

- Searches for comment start markers (`<!--`)
- Finds corresponding end markers (`-->`)
- Removes entire comment blocks including markers
- Throws exception for malformed comments (missing closing tags)

**Error Handling:** Detects and reports malformed comments with descriptive error messages.

##### transform_tags_to_lower_case()

```cpp
void transform_tags_to_lower_case(std::string &html)
```

**Purpose:** Converts HTML tag names to lowercase while preserving attribute values and content.

**Algorithm:**

1. Finds each opening tag bracket (`<`)
2. Locates the first space or closing bracket to identify tag name boundary
3. Extracts only the tag name portion (not attributes)
4. Applies `std::transform` with `::tolower` to tag name only
5. Replaces original tag name with lowercase version

**Preservation:** Carefully preserves attribute names, values, classes, and all other content.

##### remove_all_line_breaks()

```cpp
void remove_all_line_breaks(std::string &html)
```

**Purpose:** Removes all newline characters to simplify parsing logic.

**Implementation:** Uses `std::remove` and `erase` idiom for efficient character removal.

**Rationale:** Simplifies position calculations and pattern matching in subsequent parsing steps.

#### DOCTYPE Processing Functions

##### has_doctype()

```cpp
bool has_doctype(const std::string &html)
```

**Purpose:** Checks if HTML string contains a DOCTYPE declaration.

**Method:** Searches for `<!doctype` substring (case-insensitive matching).

**Returns:** Boolean indicating DOCTYPE presence.

##### extract_doctype()

```cpp
std::string extract_doctype(std::string &html)
```

**Purpose:** Extracts and removes DOCTYPE declaration from HTML string.

**Process:**

1. Locates DOCTYPE start position (`<!doctype`)
2. Finds corresponding closing bracket (`>`)
3. Extracts complete DOCTYPE string
4. Removes DOCTYPE from original HTML string
5. Returns extracted DOCTYPE for separate processing

**Side Effect:** Modifies input string by removing the DOCTYPE declaration.

#### Attribute Parsing Functions

##### parse_attributes()

```cpp
std::map<std::string, std::string> parse_attributes(std::string &attr_string)
```

**Purpose:** Parses HTML attribute string into key-value pairs.

**Algorithm:**

1. **State Machine:** Uses boolean flag `did_open_an_attribute` to track quote state
2. **Quote Handling:** Properly handles quoted attribute values with embedded spaces
3. **Key-Value Separation:** Splits on `=` character when not inside quotes
4. **Whitespace Management:** Uses `trim()` function for clean keys and values
5. **Boolean Attributes:** Handles attributes without values (assigns empty string)

**Supported Formats:**

- `class="value"` (quoted values)
- `disabled` (boolean attributes)
- `id=myid` (unquoted values)
- Mixed combinations with proper spacing

**Cleanup:** Removes empty keys and invalid entries like `/` and spaces.

##### extract_tag_and_attributes()

```cpp
std::pair<std::string, std::string> extract_tag_and_attributes(std::string &tag_content)
```

**Purpose:** Separates tag name from attribute string within tag content.

**Method:**

- Finds first space character to split tag name from attributes
- Returns pair: `{tag_name, attributes_string}`
- Handles tags without attributes gracefully

#### Tag Classification Functions

##### is_self_closing_tag()

```cpp
bool is_self_closing_tag(std::string &tag)
```

**Purpose:** Determines if a tag is a void/self-closing element.

**Implementation:**

- Uses static set for O(1) lookup performance
- Converts tag to lowercase for case-insensitive comparison
- References `get_self_closing_tags()` for the definitive list

##### get_self_closing_tags()

```cpp
std::set<std::string> get_self_closing_tags()
```

**Purpose:** Provides the authoritative list of HTML void elements.

**Included Tags:** `area`, `base`, `br`, `col`, `embed`, `hr`, `img`, `input`, `link`, `meta`, `param`, `source`, `track`, `wbr`

**Standard Compliance:** Based on HTML5 specification for void elements.

##### is_closing_tag()

```cpp
bool is_closing_tag(std::string &tag)
```

**Purpose:** Identifies closing tags (those starting with `/`).

**Logic:** Checks for `/` as first character while avoiding false positives with comments (`//`).

#### Helper Utility Functions

##### trim()

```cpp
std::string trim(std::string &str)
```

**Purpose:** Removes leading and trailing whitespace characters.

**Whitespace Characters:** Handles spaces, tabs, newlines, and carriage returns (`" \t\n\r"`).

**Algorithm:**

- Uses `find_first_not_of()` to locate content start
- Uses `find_last_not_of()` to locate content end
- Returns substring containing only meaningful content

##### decode_html_entities()

```cpp
std::string decode_html_entities(std::string &str)
```

**Purpose:** Converts HTML entities to their character equivalents.

**Supported Entities:**

- `&amp;` → `&`
- `&lt;` → `<`
- `&gt;` → `>`
- `&quot;` → `"`
- `&apos;` → `'`
- `&nbsp;` → ` ` (space)

**Method:** Iterative replacement using `std::string::replace()` with position tracking.

##### read_tag()

```cpp
std::string read_tag(const std::string &html, size_t &pos)
```

**Purpose:** Extracts tag content from current position and updates position marker.

**Process:**

1. Starts from position after opening `<`
2. Finds closing `>`
3. Extracts content between brackets
4. Updates position reference to closing bracket location

**Error Handling:** Throws exception for malformed tags (missing closing bracket).

##### fix_tag()

```cpp
std::string fix_tag(std::string &tag_with_attrs)
```

**Purpose:** Extracts clean tag name from tag content that may include attributes.

**Method:** Finds first whitespace character and returns substring before it.

**Use Case:** Separates tag name when detailed attribute parsing isn't needed.

## Examples

### Basic Element Creation

```cpp
#include <element.hpp>
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
#include <self_closing_element.hpp>
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
#include <element.hpp>
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
#include <document.hpp>
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
#include <document_parser.hpp>
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
#include <element.hpp>
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

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake 3.10 or higher
- Make (for Unix-like systems)

### Build Instructions

1. **Clone the repository:**

   ```bash
   git clone <repository-url>
   cd html-builder
   ```

2. **Using the build script (recommended):**

   ```bash
   # Make the script executable
   chmod +x run.sh

   # Clean build
   ./run.sh clean

   # Build the project
   ./run.sh

   # Build and run
   ./run.sh run
   ```

3. **Manual CMake build:**

   ```bash
   # Create build directory
   mkdir build && cd build

   # Configure
   cmake ..

   # Build
   make -j$(nproc)
   ```

### Build Configuration

The project supports several build configurations through environment variables:

- **Debug Mode:** Set `DEBUG_MODE=ON` to enable debugging symbols
- **Local Testing:** Set `LOCAL_TEST=1` to enable sanitizers and additional debugging flags

### Environment Variables

Create a `.env` file in the project root to set custom build variables:

```env
DEBUG_MODE=ON
LOCAL_TEST=1
```

## Project Structure

```
html-builder/
├── app.cpp                     # Example/test application
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # This documentation
├── run.sh                      # Build script
├── tmp.html                    # Temporary HTML file for testing
├── build/                      # Build output directory
├── includes/                   # Header files
│   ├── document.hpp            # Document class declaration
│   ├── document_parser.hpp     # Parser function declarations
│   ├── doctype_element.hpp     # DOCTYPE element class
│   ├── element.hpp             # Base element class declaration
│   └── self_closing_element.hpp # Self-closing element class
└── src/                        # Source files
    ├── document_parser.cpp     # HTML parsing implementation
    ├── element.cpp             # Base element implementation
    └── self_closing_element.cpp # Self-closing element implementation
```

### Key Files

- **`element.hpp/cpp`** - Core element class with all basic HTML element functionality
- **`self_closing_element.hpp/cpp`** - Specialized class for void HTML elements
- **`document.hpp`** - Complete HTML document representation with DOCTYPE
- **`document_parser.hpp/cpp`** - HTML parsing and template parameter substitution
- **`doctype_element.hpp`** - DOCTYPE declaration element
- **`app.cpp`** - Example usage and testing code

The library is designed to be either built as a static library or compiled directly with your application depending on the `LOCAL_TEST` environment variable configuration.
