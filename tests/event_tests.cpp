#include <nta/Event.h>
#include <nta/format.h>

using namespace std;
using namespace nta;
using namespace nta::utils;

struct Entity { int health = 100; };
struct Player : public Entity { int level = 0; };
struct Enemy : public Entity { int power; };

// The last invariant must be called COUNT so Event knows how many recipients there are
// Underlying values need to start at 0 and go up by 1 each time
enum class EntityType { ENTITY, PLAYER, ENEMY, COUNT };

// In this program, messages can be sent to Entities, Players, or Enemies
using Event = EventTemplate<EntityType, void(Entity&), void(Player&), void(Enemy&)>;

void print_state(Player p, Enemy e) {
    println("Player health {}, level {}\nEnemy  health {}, power {}\n",
            p.health, p.level, e.health, e.power);
}

// Imagine a player fighting an enemy
void test_scenario1() {
    Player p;
    for (int power = 10; p.health > 0; power += 5) {
        int prev_level = p.level;
        // Beat an enemy, gain a level
        Event level_up;
        level_up.define_for<EntityType::PLAYER>([](Player& play) {
            play.level++;
            play.health += play.level == 1 ? 0 : 25;
        });
        level_up.enact<EntityType::PLAYER>(p);
        assert(p.level == prev_level + 1);

        Enemy e = { .power = power };
        // This will do nothing because the function just define_ford cannot be
        // applied to enemies
        level_up.enact<EntityType::PLAYER>(e);
        // This will do nothing because nothing has been define_ford to ENEMY
        level_up.enact<EntityType::ENEMY>(e);

        print_state(p, e);
        while (e.health > 0) {
            // Create event that will deal damage to any entity
            Event damage;
            damage.define_for<EntityType::ENTITY>([&](Entity& ent) {
                ent.health -= e.power;
            });

            int health_before = p.health;
            // Apply event to player
            damage.enact<EntityType::ENTITY>(p);
            assert(p.health == health_before - e.power);
            if (p.health <= 0) break;

            int player_dammage = 19 + p.level;
            // Reuse damage event for player's retaliation
            damage.define_for<EntityType::ENTITY>([&](Entity& ent) {
                ent.health -= player_dammage;
            });

            health_before = e.health;
            // Can apply event using function call syntax instead
            // first parameter should usually be the type of the recipient
            //
            // This is slower than calling .enact though
            damage(EntityType::ENTITY, e);
            assert(e.health == health_before - player_dammage);

            print_state(p, e);
        }
    }
}

// Imagine a player and an enemy caught in a fire
void test_scenario2() {
    // Instead of creating different event types individually, can
    // make each event type its own class
    struct FireDamage : public Event {
        FireDamage() {
            define_for<EntityType::PLAYER>([](Player& p) {
                p.health -= 10;
            });
            // enemies are slightly fire resistant
            define_for<EntityType::ENEMY>([](Enemy& e) {
                e.health -= 5;
            });
        }
    };

    FireDamage fire;
    Player p;
    Enemy e;
    e.health = 30;

    print_state(p, e);
    while (e.health > 0) {
        int prev_e_health = e.health;
        int prev_p_health = p.health;

        // If you apply a Message using function call syntax without providing
        // the type of the recipient, then every define_ford response that takes
        // arguments of the given types will be called.
        fire(p);
        fire(e);

        print_state(p, e);
        assert(p.health == prev_p_health - 10);
        assert(e.health == prev_e_health - 5);
    }
}

void test_scenario3() {
    Event power_up;
    // Everything recovers some health when powering up
    power_up.define_for<EntityType::ENTITY>([](Entity& e) {
        e.health += 20;
    });
    // Player increase level
    power_up.define_for<EntityType::PLAYER>([](Player& p) {
        p.level++;
    });
    // Enemies increase their power stat
    power_up.define_for<EntityType::ENEMY>([](Enemy& e) {
        e.power += 5;
    });

    Player p;
    Enemy e = { .power = 10 };
    print_state(p, e);

    Player p_before = p;
    Enemy e_before = e;

    power_up(p);
    power_up(e);
    print_state(p, e);

    assert(p.health == p_before.health + 20);
    assert(p.level == p_before.level + 1);
    assert(e.health == e_before.health + 20);
    assert(e.power == e_before.power + 5);

    // They no longer gain health of power up
    power_up.undefine_for<EntityType::ENTITY>();

    p_before = p;
    e_before = e;

    power_up(p);
    power_up(e);
    print_state(p, e);

    assert(p.health == p_before.health);
    assert(p.level == p_before.level + 1);
    assert(e.health == e_before.health);
    assert(e.power == e_before.power + 5);
}

int main(int argc, char* argv[]) {
    cout<<"Running Event tests..."<<endl;

    println("Test 1");
    test_scenario1();
    println("Test 2");
    test_scenario2();
    println("Test 3");
    test_scenario3();
    
    cout<<"Tests passed"<<endl;
    return 0;
}

