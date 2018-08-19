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

    Path q("folder/file.txt");
    assert(q.exists());
    assert(q.is_file());
    assert(!q.is_directory());
    assert(q.parent() == "folder");
    assert(q.parent() == "folder/");
    assert(q.parent().is_directory());

    assert(Path("folder") + "file.txt" == q);
    assert(q.is_relative());

    Path r = "/home";
    assert(r.is_absolute());

    assert(!Path("fake/file.who").exists());

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
