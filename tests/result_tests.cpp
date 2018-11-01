#include <iostream>

#include <nta/Errors.h>

using namespace std;
using namespace nta;

Result<int> divide(int a, int b) {
    return b == 0 ? Result<int>::new_err(Error("Tried dividing by zero")) : Result<int>::new_ok(a/b);
}

int main(int argc, char* argv[]) {
    cout<<"Running Result tests..."<<endl;
    
    Result<int> res = divide(10,2);
    assert(res.is_ok());
    assert(res.unwrap() == 5);

    Result<int> res2 = divide(10,0);
    assert(res2.is_err());
    assert(res2.get_err().description == "Tried dividing by zero");
    
    cout<<"Tests passed"<<endl;
    return 0;
}
