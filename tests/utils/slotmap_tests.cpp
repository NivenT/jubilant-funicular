#include <iostream>
#include <numeric>

#include <nta/SlotMap.h>
#include <nta/format.h>

using namespace std;
using namespace nta::utils;

struct Particle {
    int val;

    Particle() : val(0) {}
    Particle(int v) : val(v) {}
};

int main(int argc, char* argv[]) {
    cout<<"Running SlotMap tests..."<<endl;

    SlotMap<Particle> map;
    assert(map.empty());

    using Key = SlotMapKey<Particle>;

    auto key = map.add({1});
    assert(map.size() == 1);
    assert(!map.is_empty());
    assert(map.capacity() >= map.size());

    auto maybe = map[key];
    assert(maybe.is_some());
    assert(maybe.unwrap().val == 1);

    maybe.unwrap().val = 10;
    assert(map.at_unsafe(key).val == 10);
    map.remove(key);
    assert(map.empty());
    assert(map[key].is_none());

    vector<pair<Key, bool>> keys;
    for (int round = 0; round < 4; ++round) {
        for (int i = 0; i < 100; i++) keys.emplace_back(map.emplace(keys.size()), true);
        assert(map.capacity() >= map.size());
        auto cap = map.capacity();

        for (int i = 0; i < keys.size(); i++) {
            if (rand()%2) {
                map.remove(keys[i].first);
                keys[i].second = false;
            }
        }
        assert(map.capacity() == cap);

        for (int i = 0; i < keys.size(); i++) {
            auto maybe = map[keys[i].first];
            assert(maybe.is_some() == keys[i].second);
            if (maybe.is_some()) assert(maybe.unwrap().val == i);
        }

        int count = 0;
        for (auto& p : map) {
            assert(0 <= p.val && p.val < keys.size());
            count++;
        }
        assert(count == map.size());
    }

    vector<bool> removed(map.capacity(), true);
    for (auto& key : keys) {
        if (key.second) removed[key.first.idx] = false;
    }
    for (int i = 0; i < removed.size(); i++) {
        assert(map.is_removed(i) == removed[i]);
    }

    auto cap = map.capacity();
    map.clear();
    assert(map.capacity() == cap);
    assert(map.is_empty());

    cout<<"Tests passed"<<endl;
    return 0;
}
