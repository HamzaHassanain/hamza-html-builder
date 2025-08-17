#include <bits/stdc++.h>
#include <document_parser.hpp>
using namespace std;
using namespace hamza_html_builder;
int main()
{

    string path = "tmp.html";
    string content;
    ifstream file(path);
    try
    {

        if (file)
        {
            stringstream buffer;
            buffer << file.rdbuf();
            content = buffer.str();
        }

        /*

         title,subtitle,heroTitle,heroDescription,email,github,linkedin
        */

        std::map<std::string, std::string> params = {
            {"title", "My Website"},
            {"subtitle", "Welcome to my website"},
            {"heroTitle", "Welcome to My Website"},
            {"heroDescription", "This is a sample description for the hero section."},
            {"contactEmail", "contact@example.com"},
            {"github", "https://github.com/username"},
            {"linkedin", "https://linkedin.com/in/username"},
            {"email", "contact@example.com"}

        };
        auto elements = parse_html_string(content);
        function<void(const vector<shared_ptr<element>> &, int)> print_recursively;
        print_recursively = [&](const vector<shared_ptr<element>> &elements, int depth)
        {
            for (const auto &element : elements)
            {
                // cout << string(depth * 2, ' ') << element->to_string() << endl;

                // print_recursively(element->get_children(), depth + 1);
            }
        };
        print_recursively(elements, 0);
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}