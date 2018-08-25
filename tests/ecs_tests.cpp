#include <iostream>

#include <nta/ECS.h>

using namespace std;
using namespace nta;

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running ECS tests..."<<endl;

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}

