#include <iostream>

#include <nta/Errors.h>

using namespace std;
using namespace nta;
using namespace nta::utils;

Result<int> divide(int a, int b) {
    return b == 0 ? Result<int>::new_err(Error("Tried dividing by zero")) : Result<int>::new_ok(a/b);
}

int main(int argc, char* argv[]) {
    cout<<"Running Result tests..."<<endl;
    
    Result<int> res = divide(10,5);
    assert(res.is_ok());
    assert(res.unwrap() == 2);

    Result<float> res2 = res.map<float>([](int n){ return n*2.0; });
    assert(res2.is_ok());
    assert(res2.unwrap() == 4.0);

    Result<int> res3 = divide(10,0);
    assert(res3.is_err());
    assert(res3.get_err().description == "Tried dividing by zero");
    
    Option<Error> res4 = res3.map([](int n){ assert(false); });
    assert(res4.is_some());
    assert(res4.unwrap().description == "Tried dividing by zero");

    auto res5 = divide(8, 2).map<float>([](int a){ return a*10; })
                            .map<bool>([](float b){ return b > 8; })
                            .map<string>([](bool c){ return c ? "true" : "false"; });
    assert(res5.is_ok() && res5.unwrap() == "true");

    cout<<"Tests passed"<<endl;
    return 0;
}
