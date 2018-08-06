#include <iostream>

#include <nta/json.h>
#include <nta/Logger.h>

using namespace std;
using namespace nta::utils;

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running Json tests..."<<endl;

    assert(Json::null().is_null());

    Json a = Json::number(8);
    Json b = 11;
    Json c = "Hello ";
    Json d;
    Json e;
    Json f = 2.5;

    assert(a.is_number());
    assert(b.is_number());
    assert(c.is_string());
    assert(d.is_null() && e.is_null());

    d["key"] = "World";
    e.push_back(false);
    e.push_back("!");

    assert(d.is_object() && e.is_array());

    double f2 = f;

    assert(a.as_number().as_uint() + b.as_uint() + f2 == 21.5);
    assert(c.as_string() + d["key"].as_string() == "Hello World");
    assert(d.as_object()["key"].as_string() + e.as_array()[1].as_string() == "World!");
    assert(!e[0].as_bool() && !e[0]);

    assert(e.size() == 2);
    assert(d.size() == 1);

    for (auto it = a.begin(); it != a.end(); ++it) {
        assert((int64_t)*it == 8);
    }
    // I love the stuff you can do in C++
    for (struct {Json::iterator<0> it; int i;} s = {e.begin(), 0}; s.it != e.end(); ++s.it, ++s.i) {
        if (s.i == 0) assert(!*s.it);
        if (s.i == 1) assert((string)*s.it == "!");
    }
    for (auto it = d.begin(); it != d.end(); ++it) {
        assert((string)*it == "World");
        assert(it.key() == "key");
        assert((string)it.value() == "World");
    }

    assert(c.dump() == "\"Hello \"");
    assert(e.dump() == "[false, \"!\"]");

    Json g;
    g["key"] = 5;
    g["key2"] = d;
    g["huh"] = e;
    g["foo"] = "bar";

    assert((string)g["key2"]["key"] == "World");
    assert(g.dump() == "{\"foo\": \"bar\", \"huh\": [false, \"!\"], \"key\": 5, \"key2\": {\"key\": \"World\"}}");
    assert(g.dump(2) == "{\n  \"foo\": \"bar\", \n  \"huh\": [false, \"!\"], \n  \"key\": 5, \n  \"key2\": {\n    \"key\": \"World\"\n  }\n}");
    assert(g.dump(2) == "{\n  \"foo\": \"bar\", \n  \"huh\": [false, \"!\"], \n  \"key\": 5, \n  \"key2\": {\n    \"key\": \"World\"\n  }\n}");

    Json h = {"This", "is", "an", "array", 42, true, Json::null()};
    assert(h.is_array());
    assert(h.dump() == "[\"This\", \"is\", \"an\", \"array\", 42, true, null]");

    Json m = {
        {"key", {"array", "in", "object"}},
        {"k", false},
        {"num", 21.5}
    };
    assert(m.is_object());
    assert(m.dump() == "{\"k\": false, \"key\": [\"array\", \"in\", \"object\"], \"num\": 21.5}");

    Json n = {};
    assert(n.is_null());

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
