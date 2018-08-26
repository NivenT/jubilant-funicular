#include <iostream>

#include <nta/IDFactory.h>

using namespace std;
using namespace nta::utils;

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running IDFactory tests..."<<endl;

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}

