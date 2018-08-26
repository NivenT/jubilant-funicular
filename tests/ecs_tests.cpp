#include <iostream>

#include <nta/ECS.h>

using namespace std;
using namespace nta;

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running ECS tests..."<<endl;

    assert(__builtin_ctz(1) == 0);
    assert(__builtin_ctz(1 << 10) == 10);

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}

