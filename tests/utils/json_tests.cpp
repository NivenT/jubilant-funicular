#include <cassert>
#include <iostream>

#include <nta/json.h>
#include <nta/Logger.h>

using namespace std;
using namespace nta::utils;

int main(int argc, char* argv[]) {
    nta::init();
    cout<<"Running Json tests..."<<endl;

    assert(Json::null().is_null());

    Json a = Json::number(8);
    Json b = 11;
    Json c = "Hello ";
    Json d;
    Json e;

    assert(a.is_number());
    assert(b.is_number());
    assert(c.is_string());
    assert(d.is_null() && e.is_null());

    d["key"] = "World";
    e.push_back(false);
    e.push_back("!");

    assert(d.is_object() && e.is_array());

    assert(a.as_number().as_uint() + b.as_uint() == 19);
    assert(c.as_string() + d["key"].as_string() == "Hello World");
    assert(d.as_object()["key"].as_string() + e.as_array()[1].as_string() == "World!");
    assert(!e[0].as_bool() && !e[0]);

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
