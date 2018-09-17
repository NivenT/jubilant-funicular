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

    assert(map.find<int>() == 5);
    assert(map.find<char>() == 'a');
    assert(map.find<int*>() == &a);
    assert(map.find<big>() == b);
    assert(map.find<char*>() == str);

    assert(map.contains<int>());
    assert(!map.contains<double>());
    assert(!map.empty());
    assert(map.size() == 5);

    *map.find<int*>() = 25;
    assert(a == 25);

    map.find<char*>()[0] = 'H';
    map.find<char*>()[2] = '\0';
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
    assert(map.find<big>().r == b.r);

    old_count = big_destructor_count;
    map.erase<big>();
    assert(big_destructor_count == old_count + 1);
    assert(map.size() == 0);

    map.insert<int&>(a);
    map.find<int&>() = 7;
    assert(a == 7);
    assert(map.find<int&>() == 7);

    assert(map.contains<int&>());
    assert(!map.contains<int>());
    assert(!map.contains<int*>());

    map.clear();
    assert(map.empty());
    map.get<int>() = 8;
    assert(map.size() == 1);
    assert(map.get<int>() == 8);

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
    assert(map.find<string>() == "bleem");
    map.find<string>() += "!";
    assert(map.find<string>() == "bleem!");

    cout<<"Tests passed"<<endl;
    return 0;
}
