#include <iostream>

#include <nta/IDFactory.h>

using namespace std;
using namespace nta::utils;

int main(int argc, char* argv[]) {
    cout<<"Running IDFactory tests..."<<endl;

    IDFactory<unsigned int> factory;
    assert(factory.gen_id() == 1);
    assert(factory() == 2);
    assert(factory() == 3);
    assert(factory.get_count() == 3);
    assert(factory.get_last_id() == 3);

    factory.free_id(1);
    assert(factory.get_count() == 2);
    assert(factory() == 1);
    assert(factory() == 4);
    assert(factory() == 5);
    assert(factory.get_last_id() == 5);

    factory.free_id(1);
    factory.free_id(3);
    assert(factory.get_count() == 3);
    assert(factory() == 3);
    assert(factory() == 1);
    assert(factory() == 6);
    assert(factory.get_count() == 6);
    assert(factory.get_last_id() == 6);

    factory.clear();
    assert(factory.get_count() == 0);
    assert(factory.get_last_id() == 0);
    assert(factory() == 1);

    cout<<"Tests passed"<<endl;
    return 0;
}

