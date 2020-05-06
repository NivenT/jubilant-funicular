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

    using Key = SlotMapKey<>;

    auto key = map.add({1});

    println("Added an element");

    assert(map.size() == 1);
    assert(!map.is_empty());
    assert(map.capacity() >= map.size());

    println("Checked size");

    auto maybe = map[key];

    println("Retrieved an element");

    assert(maybe.is_some());
    assert(maybe.unwrap().val == 1);

    println("Checked value");

    maybe.unwrap().val = 10;
    assert(map.at_unsafe(key).val == 10);
    map.remove(key);

    println("Removed an element");

    assert(map.empty());
    assert(map[key].is_none());

    println("Confirmed map is empty");

    vector<pair<Key, bool>> keys;
    for (int round = 0; round < 4; ++round) {
        println("Starting round {} of {}", round + 1, 4);

        for (int i = 0; i < 100; i++) keys.emplace_back(map.emplace(keys.size()), true);
        assert(map.capacity() >= map.size());
        auto cap = map.capacity();

        println("  Added new values");

        for (int i = 0; i < keys.size(); i++) {
            if (rand()%2) {
                map.remove(keys[i].first);
                keys[i].second = false;
            }
        }
        assert(map.capacity() == cap);

        println("  Removed some values");

        for (int i = 0; i < keys.size(); i++) {
            auto maybe = map[keys[i].first];
            assert(maybe.is_some() == keys[i].second);
            if (maybe.is_some()) assert(maybe.unwrap().val == i);
        }

        println(" Checked consistency");

        for (int i = 0; i < keys.size(); i++) {
            if (!keys[i].second && rand()%10 == 0) {
                bool free = map.is_free(keys[i].first);
                if (free) keys[i].first.gen = map.get_curr_gen(keys[i].first);
                assert(map.insert(keys[i].first, i) == free);
                if (free) {
                    keys[i].second = true;
                    assert(map[keys[i].first].unwrap().val == i);
                }
            }
        }

        println("  Inserted some values back");

        int count = 0;
        for (auto& p : map) {
            assert(0 <= p.val && p.val < keys.size());
            count++;
        }
        assert(count == map.size());

        count = 0;
        for (auto& k : keys) {
            if (k.second) count++;
        }
        assert(count == map.size());

        println("  Compared counts");
    }

    vector<bool> removed(map.capacity(), true);
    for (auto& key : keys) {
        if (key.second) removed[key.first.idx] = false;
    }
    for (int i = 0; i < removed.size(); i++) {
        assert(map.is_free(i) == removed[i]);
    }

    auto cap = map.capacity();
    map.clear();
    assert(map.capacity() == cap);
    assert(map.is_empty());

    cout<<"Tests passed"<<endl;
    return 0;
}
