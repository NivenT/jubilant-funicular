#include <iostream>

#include <nta/Path.h>

using namespace std;
using namespace nta::utils;

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running Path tests..."<<endl;

    Path p("path/to/a/file.txt");
    assert(p.to_string() == "path/to/a/file.txt");

    Path p2 = std::move(p);
    assert(p.to_string() == "");

    cout<<p<<" and "<<p2<<endl;

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
