#include <iostream>
#include <numeric>

#include <nta/StaticVector.h>
#include <nta/format.h>

using namespace std;
using namespace nta::utils;

struct NonCopyable {
    int a;
    NonCopyable(int a) : a(a) {}
    NonCopyable(const NonCopyable&) = delete;
};

struct Movable {
    int* a;
    bool moved = false;

    Movable(int* a) : a(a) {}
    Movable(const Movable& rhs) : a(rhs.a) {}
    Movable(Movable&& rhs) : a(rhs.a) { rhs.a = nullptr; rhs.moved = true; }
};

int main(int argc, char* argv[]) {
    cout<<"Running StaticVector tests..."<<endl;

    StaticVector<int, 20> v1;
    assert(v1.size() == 0);
    assert(v1.cap() == 20);
    assert(v1.is_empty());
    assert(!v1.is_full());

    for (int n : v1) assert(false);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);
    assert(v1.size() == 3);
    assert(v1[1] == 2);
    assert(v1.front() == 1);
    assert(v1.back() == 3);
    assert(v1.begin() == &v1.front());

    v1[1] = 8;
    int total = std::accumulate(std::begin(v1), std::end(v1), 0);
    assert(total == 12);
    v1.pop_back();
    total = std::accumulate(v1.begin(), v1.end(), 0);
    assert(total == 9);
    assert(v1.back() == 8);
    assert(v1.size() == 2);

    v1.insert(v1.begin(), 11);

    assert(v1.front() == 11);
    assert(v1.back() == 8);
    assert(v1.size() == 3);
    v1.insert(v1.begin() + 2, 20);
    assert(v1.back() == 8);
    assert(v1.front() == 11);
    assert(v1[1] == 1);
    assert(v1[2] == 20);
    assert(v1.size() == 4);
    total = std::accumulate(v1.begin(), v1.end(), 0);
    assert(total == 40);

    v1.clear();
    total = std::accumulate(v1.begin(), std::end(v1), 0);
    assert(total == 0);
    assert(v1.empty());

    v1.resize(20);
    assert(v1.full());
    assert(v1.size() == 20);
    std::iota(std::begin(v1), v1.end(), 0);
    for (int i = 0; i < 20; i++) assert(v1[i] == i);

    v1.remove(v1.begin() + 13);
    v1.remove(v1.begin() + 7);
    v1.remove(v1.begin() + 11);

    StaticVector<int, 17> v{0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 14, 15, 16, 17, 18, 19};
    assert(v.size() == 17);
    for (int i = 0; i < 17; i++) assert(v.at(i) == v1[i]);
    std::copy(v.begin(), v.end(), v1.rbegin());
    for (int i = 0; i < 17; i++) assert(v[i] == v1[16-i]);
    assert(v1.size() == 17);
    
    v1.clear();
    assert(v1.empty());
    v1 = v;
    assert(v1.size() == 17);
    for (int i = 0; i < 17; i++) assert(v.at(i) == v1[i]);

    StaticVector<NonCopyable, 10> v2;
    v2.emplace_back(4);
    assert(v2.front().a == 4);
    assert(v2.size() == 1);
    v2.emplace_back(8);
    v2.erase(v2.begin());
    assert(v2.front().a == 8);

    v2.emplace_back(7);
    v2.emplace(v2.begin() + 1, 10);
    assert(v2.front().a == 8);
    assert(v2[1].a == 10);
    assert(v2.back().a == 7);

    int a = 0;
    StaticVector<Movable, 15> v3(5, Movable(&a)); // copies, doesn't move
    for (auto& m : v3) assert(!m.moved);

    StaticVector<Movable, 8> w(std::move(v3));
    for (auto& m : v3) assert(m.a == nullptr && m.moved);
    for (auto& m : w) assert(m.a == &a && !m.moved);
    *w.front().a = 10;
    assert(a == 10);

    cout<<"Tests passed"<<endl;
    return 0;
}
