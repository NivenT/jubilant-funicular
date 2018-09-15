#include <iostream>

#include <nta/TypeMap.h>

using namespace std;
using namespace nta::utils;

int big_destructor_count = 0;

struct big {
    ~big() { big_destructor_count++; }
    bool operator==(const big& rhs) const { return r == rhs.r && i == rhs.i; }
    double r, i;
};

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running TypeMap tests..."<<endl;

    int a = 11;
    big b = {3.5, 7.25};
    char* str = strdup("hello");

    TypeMap map;
    map.insert<int>(5);
    map.insert<char>('a');
    map.insert<int*>(&a);
    map.insert<big>(b);
    map.insert<char*>(str);

    assert(map.get<int>() == 5);
    assert(map.get<char>() == 'a');
    assert(map.get<int*>() == &a);
    assert(map.get<big>() == b);
    assert(map.get<char*>() == str);

    assert(map.contains<int>());
    assert(!map.contains<double>());
    assert(!map.empty());
    assert(map.size() == 5);

    *map.get<int*>() = 25;
    assert(a == 25);

    map.get<char*>()[0] = 'H';
    map.get<char*>()[2] = '\0';
    assert(strcmp(str, "He") == 0);

    // TypeMap does not do memory management for you
    free(str);
    int old_count = big_destructor_count;
    map.clear();
    // TypeMap runs destructors when you call clear
    assert(big_destructor_count == old_count + 1);

    assert(!map.contains<big>());
    assert(map.is_empty());
    assert(map.size() == 0);

    map.insert<big>(b);
    assert(map.get<big>().r == b.r);

    old_count = big_destructor_count;
    map.erase<big>();
    assert(big_destructor_count == old_count + 1);
    assert(map.size() == 0);

    map.insert<int&>(a);
    map.get<int&>() = 7;
    assert(a == 7);
    assert(map.get<int&>() == 7);

    assert(map.contains<int&>());
    assert(!map.contains<int>());
    assert(!map.contains<int*>());

    map.clear();
    map.insert<int*>(new int(4));
    map.insert<char*>(new char('3'));
    map.insert<float*>(new float(3.4));
    map.insert<string>("bleem");

    // bad example of iteratoring over a TypeMap, but the point is that you can iterator over a TypeMap
    for (auto& pair : map) {
        if (pair.first.hash == typeid(int*).hash_code()) { 
            delete (int*)pair.second;
        } else if (pair.first.hash == typeid(char*).hash_code()) {
            delete (char*)pair.second;
        } else if (pair.first.hash == typeid(float*).hash_code()) {
            delete (float*)pair.second;
        }
    }
    assert(map.get<string>() == "bleem");
    map.get<string>() += "!";
    assert(map.get<string>() == "bleem!");

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
