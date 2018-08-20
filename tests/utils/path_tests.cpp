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
    assert(p.is_empty());

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
    assert(Path("C:\\").is_absolute());

    Path cwd = Path::cwd();
    assert(cwd.is_directory() && cwd.is_absolute());
    assert(cwd + "folder" == Path("folder/..///folder/./").resolve());

    assert(Path("Desktop\\Stuff\\And\\Things") == Path("Desktop/Stuff/And/Things"));

    Path s("fake");
    assert(!s.exists());
    for (const Path& p : s) {
    	assert(false);
    }

    for (const auto& p : q) {
    	assert(p == q);
    }

    Path t = "folder";
    auto it = t.begin();
    assert(*(it++) == ".");
    assert(*(it++) == "..");
    assert(*(it++) == "file");
    assert(*(it++) == "file.txt");
    assert(it == t.end());

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
