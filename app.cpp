#include <bits/stdc++.h>

using namespace std;

int main()
{

    // read the file tmp.html

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

        cout << content << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}