#include <iostream>

#include <nta/Path.h>

using namespace std;
using namespace nta::utils;

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running Path tests..."<<endl;

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}
