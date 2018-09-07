#include <iostream>

#include <nta/Wrapper.h>
#include <nta/utils.h>

using namespace std;
using namespace nta;

NTA_CREATE_WRAPPER(Inches, int)
NTA_CREATE_WRAPPER(Feet, int)
NTA_CREATE_WRAPPER(Name, string)

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running Wrapper tests..."<<endl;

    Inches a = 12;
    Feet b = 1;
    Name c("Steve");

    assert(utils::to_string(a) == "Inches(12)");
    assert(utils::to_string(b) == "Feet(1)");
    assert(utils::to_string(c) == "Name(Steve)");

    // Confirms that you can't check for equality between Inches and Feet even though they wrap the same type
    if ((int)check::EqualsExists<Inches, Feet>::value == 1) {
        assert(false);
    }

    assert(a == 12);
    assert(a + 5 == 17);
    assert((b << 3) == 8);
    assert(a++ == 12);
    assert(a == 13);
    assert(++b == 2);
    assert(a * 9 == 117);
    assert(b == 2);
    assert(b / 2 == Feet(1));
    assert((a & 8) == 8);
    assert((b | 4) == 6);
    assert(~Inches(0) == Inches(~0));
    assert(--a == 12);
    assert((a >> 1) == 6);
    assert(a % 3 == 0);
    assert(b % 3 == 2);

    a *= 5;
    assert(a == 60);
    b /= 2;
    assert(b == 1);
    a -= 15;
    assert(a == 45);
    b += 8;
    assert(b == 9);
    assert(a != 10);

    assert(a > 3);
    assert(b < 10);
    assert(a <= 45);
    assert(b >= 8);

    assert(c + string(" Rogers") == Name("Steve Rogers"));
    // As a quirk of the implementation, get an empty wrapper whenever
    // an invalid operation is performed
    assert(c - Name("Uh-oh") == Name());

    c += string("n Strange");
    assert(utils::to_string(c) == "Name(Steven Strange)");
    assert(c == c.to_inner());
    if (!is_same<Name::inner, string>::value) {
        assert(false);
    }
    if (!is_same<decltype(c.to_inner()), string>::value) {
        assert(false);
    }

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}

