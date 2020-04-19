#include <nta/ECS.h>
#include <nta/format.h>

using namespace std;
using namespace nta;
using namespace nta::utils;

class HealthComponent : public Component {
private:
	int m_health;
public:
	HealthComponent() : m_health(100) {}
	int get_health() const { return m_health; }
    void hurt(int dam) { m_health -= dam; }
};

class DamageComponent : public Component {
private:
    long m_damage;
public:
	DamageComponent(long dam = 0) : m_damage(dam) {}
    long get_damage() const { return m_damage; }
};

enum class ComponentType { HEALTH, DAMAGE, COUNT };
using Event = EventTemplate<ComponentType, void(HealthComponent&), void(DamageComponent&)>;

class FakeComponent : public Component {
};

int main(int argc, char* argv[]) {
    cout<<"Running ECS tests..."<<endl;
    
    ComponentRegistry registry;
    registry.register_component<HealthComponent>();
    registry.register_component<DamageComponent>();
    registry.register_component<FakeComponent>();
    
    ECS system(registry);
    Entity healthy = system.gen_entity();
    Entity sick = system.gen_entity();
    Entity empty = system.gen_entity();
    Entity noone = { .idx = empty.idx + 10 };

    println("healthy = { .idx = {}, .gen = {} }", healthy.idx, healthy.gen);
    println("sick = { .idx = {}, .gen = {} }", sick.idx, sick.gen);
    println("empty = { .idx = {}, .gen = {} }", empty.idx, empty.gen);

    assert(system.does_entity_exist(healthy));
    assert(system.does_entity_exist(sick));
    assert(system.does_entity_exist(empty));
    assert(!system.does_entity_exist(noone));

    ComponentID sick_h = system.add_component<HealthComponent>(sick).unwrap();
    println("Added component: {}", sick_h);
    ComponentID sick_d = system.add_component<DamageComponent>(sick, 5).unwrap();
    println("Added component: {}", sick_d);
    ComponentID healthy_h = system.add_component<HealthComponent>(healthy).unwrap();
    println("Added component: {}", healthy_h);

    assert(system.get_owner(sick_d).is_some());
    assert(system.get_owner(sick_h) == sick);
    assert(system.has_component<DamageComponent>(sick));

    for (int sick_health = 100; sick_health; sick_health -= 5) {
        SlotMap<HealthComponent>& healths = system.get_component_list<HealthComponent>();
        
        // No guarantee about the order of the components
        for (auto& h : healths) {
            assert(h.get_health() == sick_health || h.get_health() == 100);
        }
        assert(healths.size() == 2);

        // You can loop over components with for_each
        system.for_each<DamageComponent>([&](DamageComponent& dam) {
            system.get_sibling<HealthComponent>(dam.get_id()).map([&](HealthComponent& h) {
                h.hurt(dam.get_damage());
            });
        });
        assert(system.has_sibling<DamageComponent>(sick_h));

        for (HealthComponent& h : healths) {
            if (system.has_sibling<DamageComponent>(h.get_id())) {
                assert(h.get_health() == sick_health - 5);
            } else {
                assert(h.get_health() == 100);
            }
        }

        // or by manually requesting a vector of them
        for (auto& d : system.get_component_list<DamageComponent>()) {
            assert(d.get_damage() == 5);
        }
        assert(system.num_components<DamageComponent>() == 1);
    }
    
    assert(!system.has_component<DamageComponent>(healthy));
    assert(system.has_component<HealthComponent>(sick));
    assert(!system.has_component<FakeComponent>(healthy));
    
    auto owner = system.get_owner(sick_d);
    assert(owner.is_some() && owner.unwrap() == sick);

    assert(system.get_component<HealthComponent>(sick).unwrap().get_health() == 0);
    assert(system.get_component<HealthComponent>(healthy).unwrap().get_health() == 100);

    assert(system.delete_component(sick_d));
    assert(!system.has_component<DamageComponent>(sick));

    assert(system.add_component<DamageComponent>(sick).is_some());
    // Only 1 component of each time
    assert(system.add_component<DamageComponent>(sick).is_none());
    
    assert(system.delete_component(system.get_component<DamageComponent>(sick).unwrap().get_id()));
    assert(!system.has_component<DamageComponent>(sick));
    
    assert(system.add_component<HealthComponent>(noone).is_none());
    
    assert(system.does_entity_exist(sick));
    assert(system.does_entity_exist(healthy));
    assert(system.does_entity_exist(empty));
    assert(system.delete_entity(healthy));
    assert(!system.does_entity_exist(healthy));
    assert(!system.does_entity_exist(noone));

    assert(system.get_component<DamageComponent>(healthy).is_none());
    assert(system.get_component<HealthComponent>(empty).is_none());
    assert(system.get_component<DamageComponent>(empty).is_none());
    assert(system.get_component<FakeComponent>(empty).is_none());
    
    cout<<"Tests passed"<<endl;
    return 0;
}

